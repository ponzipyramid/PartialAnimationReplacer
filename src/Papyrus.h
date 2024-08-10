#pragma once

#include "ReplacerManager.h"

constexpr std::string_view PapyrusClass = "PartialAnimationReplacer";

namespace
{
	inline bool Reload(RE::StaticFunctionTag*, std::string a_dir, std::string a_name)
	{
		if (!a_name.ends_with(".json")) {
			a_name += ".json";
		}

		fs::directory_entry entry{ "Data\\SKSE\\PartialAnimationReplacer\\Replacers\\" + a_dir + "\\" + a_name };
		logger::info("Reload {} {} {}", a_dir, a_name, entry.path().string());
		return PAR::ReplacerManager::ReloadFile(entry);
	}

	inline bool Dump(RE::StaticFunctionTag*, RE::Actor* a_actor, std::string a_dir, std::string a_name)
	{
		if (!a_name.ends_with(".json")) {
			a_name += ".json";
		}

		logger::info("Dump {} {} {}", a_actor->GetFormID(), a_dir, a_name);
		return PAR::ReplacerManager::Dump(a_actor, a_dir, a_name);
	}
}

namespace PAR::Papyrus
{
	inline bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
	{
		#define REGISTERPAPYRUSFUNC(name) vm->RegisterFunction(#name, PapyrusClass, name);

		REGISTERPAPYRUSFUNC(Reload)
		REGISTERPAPYRUSFUNC(Dump)

		return true;
	}
}