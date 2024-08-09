#pragma once

#include "Replacer.h"

namespace PAR
{
	class ReplacerManager
	{
	public:
		static void Init();
		static void LoadDir(const fs::directory_entry& a_dir);
		static bool LoadFile(const fs::directory_entry& a_file);
		static void LoadNodes();

		static void ApplyReplacers(RE::NiAVObject* a_playerObj);
		static void EvaluateReplacers();

		static void Dump(RE::Actor* a_actor, std::string a_dir, std::string a_name);

	private:
		typedef std::unordered_map<RE::FormID, Replacer*> ReplacerMap;

		static Replacer* FindReplacer(RE::Actor* a_actor);
		static bool ApplyReplacer(const std::shared_ptr<ReplacerMap>& a_map, RE::FormID a_id, RE::NiAVObject* a_obj);

		static inline std::vector<Replacer> _replacers;
		static inline std::map<std::string, std::size_t> _paths;
		static inline std::vector<std::string> _armNodes;

		static inline std::atomic<std::shared_ptr<ReplacerMap>> _current;
	};
}