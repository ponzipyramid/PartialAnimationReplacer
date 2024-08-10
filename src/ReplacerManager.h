#pragma once

#include "Replacer.h"

namespace PAR
{
	class ReplacerManager
	{
	public:
		static void Init();
		
		static bool ReloadFile(const fs::directory_entry& a_file);
		static void LoadNodes();

		static void ApplyReplacers(RE::NiAVObject* a_playerObj);
		static void EvaluateReplacers();

		static bool Dump(RE::Actor* a_actor, std::string a_dir, std::string a_name);

	private:
		static void LoadDir(const fs::directory_entry& a_dir);
		static bool LoadFile(const fs::directory_entry& a_file);

		typedef std::unordered_map<RE::FormID, std::shared_ptr<Replacer>> ReplacerMap;

		static std::shared_ptr<Replacer> FindReplacer(RE::Actor* a_actor);
		static bool ApplyReplacer(const std::shared_ptr<ReplacerMap>& a_map, RE::FormID a_id, RE::NiAVObject* a_obj);

		static inline std::vector<std::shared_ptr<Replacer>> _replacers;
		static inline std::map<std::string, std::size_t> _paths;
		static inline std::vector<std::string> _armNodes;

		static inline std::mutex _mutex;
		static inline std::atomic<std::shared_ptr<ReplacerMap>> _current;
	};
}