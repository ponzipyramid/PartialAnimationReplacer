#include "OverrideManager.h"

constexpr std::string_view PapyrusClass = "NodeManipulator";

namespace
{
	inline void Reload(RE::StaticFunctionTag*)
	{
		OverrideManager::Init();
		return;
	}

	inline void DumpRotation(RE::StaticFunctionTag*, RE::Actor* a_actor)
	{
		if (const auto obj = a_actor->Get3D(false)) {
			
		}
	}
}

namespace Papyrus
{
	inline bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
	{
		vm->RegisterFunction("Reload", PapyrusClass, Reload);
		return true;
	}
}