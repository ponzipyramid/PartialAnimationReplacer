#include "OverrideManager.h"

constexpr std::string_view PapyrusClass = "NodeManipulator";

namespace
{
	inline void Reload(RE::StaticFunctionTag*)
	{
		OverrideManager::Init();
		return;
	}
}

namespace Papyrus
{
	inline bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
	{
		vm->RegisterFunction("Reload", PapyrusClass, Reload);
		return true;
	}
};