#pragma once

#include "ConditionParser.h"

namespace PAR
{
	struct Override
	{
		std::string name;
		RE::NiMatrix3 rot;
		RE::NiPoint3 translate;
		float scale;
	};

	typedef std::vector<Override> Frame;

	struct ReplacerData
	{
		uint64_t priority;
		std::vector<Frame> frames;
		std::vector<std::string> conditions;
		std::unordered_map<std::string, std::string> refs;
	};

	class Replacer
	{
	public:
		Replacer(const ReplacerData& a_raw)
		{
			_priority = a_raw.priority;
			_frames = a_raw.frames;
			
			_rawRefs = a_raw.refs;
			for (const auto& [key, ref] : _rawRefs) {
				_refs[key] = Util::GetFormFromString(ref);
			}
			
			_rawConditions = a_raw.conditions;
			auto condition = std::make_shared<RE::TESCondition>();
			RE::TESConditionItem** head = std::addressof(condition->head);
			int numConditions = 0;
			for (auto& text : _rawConditions) {
				if (text.empty())
					continue;

				if (auto conditionItem = ConditionParser::Parse(text, _refs)) {
					*head = conditionItem;
					head = std::addressof(conditionItem->next);
					numConditions += 1;
				} else {
					logger::info("Aborting condition parsing"sv);
					numConditions = 0;
					break;
				}
			}

			_conditions = numConditions ? condition : nullptr;
		}
		inline ReplacerData GetData()
		{
			return ReplacerData{ _priority, _frames, _rawConditions, _rawRefs };
		}
		inline void Apply(RE::NiAVObject* a_obj) const
		{
			const auto& overrides = _frames[0];

			for (const auto& override : overrides) {
				if (const auto node = a_obj->GetObjectByName(override.name)) {
					node->local.rotate = override.rot;
					node->local.translate = override.translate;
					node->local.scale = override.scale;
				}
			}
		}
		inline bool Eval(RE::Actor* a_actor) const
		{
			return _conditions != nullptr && _conditions->IsTrue(a_actor, a_actor);
		}
		inline bool IsValid(const std::string& a_file) const
		{
			bool valid = true;

			if (!_conditions) {
				logger::error("{}: must have conditions", a_file);
			}

			if (_frames.empty()) {
				logger::error("{}: no frames found", a_file);
				valid = false;
			}

			for (int i = 0; i < _frames.size(); i++) {
				const auto& frame = _frames[i];
				if (frame.empty()) {
					logger::error("{}: no overrides defined in frame at {}", a_file, i);
					valid = false;
				}
				for (const auto& override : frame) {
					if (override.name.empty()) {
						logger::error("{}: override with no node found in frame at {}", a_file, i);
						valid = false;
						break;
					}
				}
			}

			return valid;
		}
	private:
		uint64_t _priority;
		std::vector<std::vector<Override>> _frames;

		std::vector<std::string> _rawConditions;
		std::shared_ptr<RE::TESCondition> _conditions = nullptr;

		std::unordered_map<std::string, std::string> _rawRefs;
		ConditionParser::RefMap _refs;
	};

	inline void from_json(const json& j, Override& o)
	{
		o.name = j.value("name", "");

		o.rot = RE::NiMatrix3{ 
			j["rotate"].value("x", 0.f), 
			j["rotate"].value("y", 0.f), 
			j["rotate"].value("z", 0.f) 
		};

		o.translate = RE::NiPoint3{
			j["translate"].value("x", 0.f),
			j["translate"].value("y", 0.f),
			j["translate"].value("z", 0.f)
		};

		o.scale = j.value("scale", 1.f);
	}

	inline void to_json(json& j, const Override& o)
	{
		RE::NiPoint3 rot;
		o.rot.ToEulerAnglesXYZ(rot);

		j = json{ 
			{ "name", o.name }, 
			{ "rotate",
				json{
					{ "x", rot.x },
					{ "y", rot.y },
					{ "z", rot.z },
				} 
			},
			{ "translate",
				json{
					{ "x", o.translate.x },
					{ "y", o.translate.y },
					{ "z", o.translate.z },
				} 
			},
			{ "scale", o.scale }
		};
	}

	inline void from_json(const json& j, ReplacerData& r)
	{
		r.priority = j.value("priority", 0);
		r.frames = j.value("frames", std::vector<Frame>{});
		r.conditions = j.value("conditions", std::vector<std::string>{});
		r.refs = j.value("refs", std::unordered_map<std::string, std::string>{});
	}

	inline void to_json(json& j, const ReplacerData& r)
	{
		j = json{
			{ "priority", r.priority },
			{ "refs", r.refs },
			{ "conditions", r.conditions },
			{ "frames", r.frames }
		};
	}
}