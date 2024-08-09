#pragma once


namespace PAR
{
	struct Override
	{
		std::string name;
		RE::NiMatrix3 rot;
	};

	typedef std::vector<Override> Frame;

	struct ReplacerData
	{
		uint64_t priority;
		std::vector<Frame> frames;
	};

	class Replacer
	{
	public:
		Replacer(const ReplacerData& a_raw)
		{
			_priority = a_raw.priority;
			_frames = a_raw.frames;
		}
		inline ReplacerData GetData()
		{
			return ReplacerData{ _priority, _frames };
		}
		inline void Apply(RE::NiAVObject* a_obj) const
		{
			const auto& overrides = _frames[0];

			for (const auto& override : overrides) {
				if (const auto node = a_obj->GetObjectByName(override.name)) {
					node->local.rotate = override.rot;
				}
			}
		}
		inline bool Eval(RE::Actor* a_actor) const
		{
			return _conditions == nullptr || _conditions->IsTrue(a_actor, a_actor);
		}
		inline bool IsValid(const std::string& a_file) const
		{
			bool valid = true;

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
		std::shared_ptr<RE::TESCondition> _conditions = nullptr;
	};

	inline void from_json(const json& j, Override& o)
	{
		o.name = j.value("name", "");

		const auto x = j.value("x", 0.f);
		const auto y = j.value("y", 0.f);
		const auto z = j.value("z", 0.f);

		o.rot = RE::NiMatrix3{ x, y, z };
	}

	inline void to_json(json& j, const Override& o)
	{
		float x, y, z;
		o.rot.ToEulerAnglesXYZ(x, y, z);

		j = json{ { "name", o.name }, { "x", x }, { "y", y }, { "z", z } };
	}

	inline void from_json(const json& j, ReplacerData& r)
	{
		r.priority = j.value("priority", 0);
		r.frames = j.value("frames", std::vector<Frame>{});
	}

	inline void to_json(json& j, const ReplacerData& r)
	{
		j = json{ { "priority", r.priority }, { "frames", r.frames } };
	}
}