#pragma once

#include "ConditionParser.h"

namespace PAR
{
	struct Override
	{
		std::string name;
		RE::NiTransform transform;
	};

	typedef std::vector<Override> Frame;

	struct ReplacerData
	{
		uint64_t priority;
		std::vector<Frame> frames;
		
		bool rotate;
		bool translate;
		bool scale;

		std::vector<std::string> conditions;
		std::unordered_map<std::string, std::string> refs;
	};

	class Replacer
	{
	public:
		Replacer(const ReplacerData& a_raw) :
			_priority(a_raw.priority), _frames(a_raw.frames), _rotate(a_raw.rotate), _translate(a_raw.translate), _scale(a_raw.scale)
		{	
			for (const auto& [key, ref] : a_raw.refs) {
				_refs[key] = Util::GetFormFromString(ref);
			}
			
			auto condition = std::make_shared<RE::TESCondition>();
			RE::TESConditionItem** head = std::addressof(condition->head);
			int numConditions = 0;
			for (auto& text : a_raw.conditions) {
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
			return ReplacerData{ _priority, _frames, _rotate, _translate, _scale };
		}
		inline void Apply(RE::NiAVObject* a_obj) const
		{
			const auto& overrides = _frames[0];

			for (const auto& override : overrides) {
				if (const auto node = a_obj->GetObjectByName(override.name)) {
					if (_rotate) {
						node->local.rotate = override.transform.rotate;
					}
					if (_translate) {
						node->local.translate = override.transform.translate;
					}
					if (_scale) {
						node->local.scale = override.transform.scale;
					}
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

		bool _rotate;
		bool _translate;
		bool _scale;

		std::shared_ptr<RE::TESCondition> _conditions = nullptr;
		ConditionParser::RefMap _refs;
	};

	inline void from_json(const json& j, Override& o)
	{
		o.name = j.value("name", "");

		const auto val = j["rotate"].get<std::vector<std::vector<float>>>();

		for (int i = 0; i < 3; i++) {
			for (int k = 0; k < 3; k++) {
				o.transform.rotate.entry[i][k] = val[i][k];
			}
		}

		o.transform.translate = RE::NiPoint3{
			j["translate"].value("x", 0.f),
			j["translate"].value("y", 0.f),
			j["translate"].value("z", 0.f)
		};

		o.transform.scale = j.value("scale", 1.f);
	}

	inline void to_json(json& j, const Override& o)
	{
		j = json{ 
			{ "name", o.name }, 
			{ "rotate", json{ o.transform.rotate.entry } },
			{ "translate",
				json{
					{ "x", o.transform.translate.x },
					{ "y", o.transform.translate.y },
					{ "z", o.transform.translate.z },
				} 
			},
			{ "scale", o.transform.scale }
		};
	}

	inline void from_json(const json& j, ReplacerData& r)
	{
		r.priority = j.value("priority", 0);
		r.frames = j.value("frames", std::vector<Frame>{});
		r.conditions = j.value("conditions", std::vector<std::string>{});
		r.refs = j.value("refs", std::unordered_map<std::string, std::string>{});
		r.rotate = j.value("rotate", true);
		r.translate = j.value("translate", false);
		r.scale = j.value("scale", false);
	}

	inline void to_json(json& j, const ReplacerData& r)
	{
		j = json{
			{ "priority", r.priority },
			{ "rotate", r.rotate },
			{ "translate", r.translate },
			{ "scale", r.scale },
			{ "refs", r.refs },
			{ "conditions", r.conditions },
			{ "frames", r.frames }
		};
	}
}