#pragma once

#include "ReplacerManager.h"
#include "Dumper.h"

constexpr std::string_view PapyrusClass = "PartialAnimationReplacer";

using namespace PAR;

namespace
{
	inline void SetEnabled(RE::StaticFunctionTag*, bool a_enabled)
	{
		ReplacerManager::SetEnabled(a_enabled);
	}

	inline bool Reload(RE::StaticFunctionTag*, std::string a_dir, std::string a_name)
	{
		if (!a_name.ends_with(".json")) {
			a_name += ".json";
		}

		fs::directory_entry entry{ "Data\\SKSE\\PartialAnimationReplacer\\Replacers\\" + a_dir + "\\" + a_name };
		return ReplacerManager::ReloadFile(entry);
	}

	inline bool Dump(RE::StaticFunctionTag*, RE::Actor* a_actor, std::string a_dir, std::string a_name, std::string a_nodes, int a_target, bool a_rotate, bool a_translate, bool a_scale)
	{
		if (!a_name.ends_with(".json")) {
			a_name += ".json";
		}

		if (!a_nodes.ends_with(".json")) {
			a_nodes += ".json";
		}

		return Dumper::QueueDump(a_actor, a_dir, a_name, a_nodes, a_target, a_rotate, a_translate, a_scale);
	}
}

namespace PAR::Papyrus
{
	inline bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
	{
		#define REGISTERPAPYRUSFUNC(name) vm->RegisterFunction(#name, PapyrusClass, name);

		REGISTERPAPYRUSFUNC(SetEnabled)
		REGISTERPAPYRUSFUNC(Reload)
		REGISTERPAPYRUSFUNC(Dump)

		return true;
	}
}