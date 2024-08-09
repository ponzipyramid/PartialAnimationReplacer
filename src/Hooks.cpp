#include "Hooks.h"
#include "ReplacerManager.h"

using namespace PAR;

namespace
{
	constexpr float TIME_DELTA = 2.f;

	struct UpdateThirdPerson
	{
		static void thunk(RE::NiAVObject* a_obj, RE::NiUpdateData* updateData)
		{
			ReplacerManager::ApplyReplacers(a_obj);
			func(a_obj, updateData);
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t size{ 5 };
	};
}

void Hooks::Install()
{
	stl::write_thunk_call<UpdateThirdPerson>(REL::RelocationID(39446, 40522).address() + 0x94);

	REL::Relocation<std::uintptr_t> vtbl1{ RE::PlayerCharacter::VTABLE[0] };
	_UpdatePlayer = vtbl1.write_vfunc(REL::Module::GetRuntime() != REL::Module::Runtime::VR ? 0x0AD : 0x0AF, UpdatePlayer);

	logger::info("Hooks installed");
}

void Hooks::UpdatePlayer(RE::Actor* a_actor, float a_delta)
{
	_UpdatePlayer(a_actor, a_delta);
	_lastUpdated += a_delta;
	if (_lastUpdated >= TIME_DELTA) {
		
		std::thread([]() {
			ReplacerManager::EvaluateReplacers();
		}).detach();

		_lastUpdated = 0.f;
	}
}