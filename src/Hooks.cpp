#include "Hooks.h"

void Hooks::Install()
{
	REL::Relocation<std::uintptr_t> target{ REL::RelocationID(19316, 19743), REL::VariantOffset(0x0, 0x0, 0x0) };

	const uintptr_t addr = target.address();

	_Update3D = (Update3DType)addr;
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)_Update3D, (PBYTE)&Update3D);

	if (DetourTransactionCommit() == NO_ERROR) {
		logger::info("Installed hook on Update3D");
	} else {
		logger::info("Failed to install hook on Update3D");
	}
}

bool Hooks::Update3D(RE::Actor* a_actor)
{
	logger::info("Hooks:Update3D");

	return _Update3D(a_actor);
}