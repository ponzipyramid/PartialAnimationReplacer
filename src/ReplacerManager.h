#pragma once

#include "Replacer.h"

namespace PAR
{
	class ReplacerManager
	{
	public:
		static void Init();
		static void ApplyReplacers(RE::NiAVObject* a_playerObj);
		static void EvaluateReplacers();

	private:
		typedef std::unordered_map<RE::FormID, Replacer*> ReplacerMap;

		static Replacer* FindReplacer(RE::Actor* a_actor);
		static bool ApplyReplacer(const std::shared_ptr<ReplacerMap>& a_map, RE::FormID a_id, RE::NiAVObject* a_obj);

		static inline std::vector<Replacer> _replacers;
		static inline std::atomic<std::shared_ptr<ReplacerMap>> _current;
	};
}