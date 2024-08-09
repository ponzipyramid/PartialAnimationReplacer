#include "ReplacerManager.h"


constexpr std::string_view PapyrusClass = "NodeManipulator";

namespace
{
	inline void Reload(RE::StaticFunctionTag*)
	{
		PAR::ReplacerManager::Init();
		return;
	}
}

namespace PAR::Papyrus
{
	inline bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
	{
		vm->RegisterFunction("Reload", PapyrusClass, Reload);
		return true;
	}
}