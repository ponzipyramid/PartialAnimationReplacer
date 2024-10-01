#pragma once

#include "Replacer.h"

namespace PAR
{
	typedef std::unordered_map<RE::FormID, std::shared_ptr<Replacer>> ReplacerMap;
	
	class ReplacerManager
	{
	public:
		static void Init();
		
		static bool ReloadFile(const fs::directory_entry& a_file);

		static void ApplyReplacers(RE::NiAVObject* a_playerObj);
		static void EvaluateReplacers();

		static void SetEnabled(bool a_enabled) { _enabled = a_enabled; }
	private:
		static void LoadDir(const fs::directory_entry& a_dir);
		static bool LoadFile(const fs::directory_entry& a_file);

		static std::shared_ptr<Replacer> FindReplacer(RE::Actor* a_actor);
		static bool ApplyReplacer(const std::shared_ptr<ReplacerMap>& a_map, RE::FormID a_id, RE::NiAVObject* a_obj);

		static void Sort();

		static inline std::vector<std::shared_ptr<Replacer>> _replacers;
		static inline std::map<std::string, std::size_t> _paths;

		static inline bool _enabled = true;

		static inline std::mutex _mutex;
		static inline std::atomic<std::shared_ptr<ReplacerMap>> _current;
	};
}