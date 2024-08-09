#pragma once

#include "ReplacerManager.h"

constexpr std::string_view PapyrusClass = "NodeManipulator";

namespace
{
	inline void ReloadDir(RE::StaticFunctionTag*, std::string a_name)
	{
		fs::directory_entry entry{ "Data\\SKSE\\PartialAnimationReplacer\\Replacers\\" + a_name };
		PAR::ReplacerManager::LoadDir(entry);
	}

	inline void ReloadFile(RE::StaticFunctionTag*, std::string a_dir, std::string a_name)
	{
		fs::directory_entry entry{ "Data\\SKSE\\PartialAnimationReplacer\\Replacers\\" + a_dir + "\\" + a_name };
		PAR::ReplacerManager::LoadFile(entry);
	}

	inline void Dump(RE::StaticFunctionTag*, RE::Actor* a_actor, std::string a_dir, std::string a_name)
	{
		PAR::ReplacerManager::Dump(a_actor, a_dir, a_name);
	}
}

namespace PAR::Papyrus
{
	inline bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
	{
		#define REGISTERPAPYRUSFUNC(name) vm->RegisterFunction(#name, PapyrusClass, name);

		REGISTERPAPYRUSFUNC(ReloadDir)
		REGISTERPAPYRUSFUNC(ReloadFile)
		REGISTERPAPYRUSFUNC(Dump)

		return true;
	}
}