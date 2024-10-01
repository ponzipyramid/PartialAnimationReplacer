#pragma once

#include "DumpJob.h"

namespace PAR
{
	class Dumper
	{
	public:
		static bool QueueDump(RE::Actor* a_actor, std::string a_dir, std::string a_name, std::string a_nodes, int a_target, bool a_rotate, bool a_translate, bool a_scale);
		static void OnFrame();
	private:
		static inline std::unordered_map<std::string, DumpJob> _jobs;
		static inline std::mutex _mutex;
	};
}