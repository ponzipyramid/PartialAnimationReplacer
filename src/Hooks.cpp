#include "Hooks.h"
#include "ReplacerManager.h"
#include "Dumper.h"

using namespace PAR;

namespace
{
	constexpr float TIME_DELTA = 1.f;

	void PrintRotation(RE::NiAVObject* a_obj)
	{
		if (const auto node = a_obj->GetObjectByName("NPC R Forearm [RLar]")) {
			float x;
			float y;
			float z;
			node->local.rotate.ToEulerAnglesXYZ(x, y, z);
			logger::info("Rotation: {} {} {}", x, y, z);
		}
	}

	struct UpdateThirdPerson
	{
		static void thunk(RE::NiAVObject* a_obj, RE::NiUpdateData* updateData)
		{
			//logger::info("start");
			//PrintRotation(a_obj);
			ReplacerManager::ApplyReplacers(a_obj);
			//PrintRotation(a_obj);
			func(a_obj, updateData);
			Dumper::OnFrame();
		}
		static inline REL::Relocation<decltype(thunk)> func;
		static inline constexpr std::size_t size{ 5 };
	};
}

void Hooks::Install()
{
	stl::write_thunk_call<UpdateThirdPerson>(REL::RelocationID(39446, 40522).address() + 0x94);

	REL::Relocation<std::uintptr_t> vtbl{ RE::PlayerCharacter::VTABLE[0] };
	_UpdatePlayer = vtbl.write_vfunc(REL::Module::GetRuntime() != REL::Module::Runtime::VR ? 0x0AD : 0x0AF, UpdatePlayer);

	logger::info("Hooks installed");
}

void Hooks::UpdatePlayer(RE::Actor* a_actor, float a_delta)
{
	_UpdatePlayer(a_actor, a_delta);
	_lastUpdated += a_delta;

	if (!_loaded || _lastUpdated >= TIME_DELTA) {
		_loaded = true;
		_lastUpdated = 0.f;

		std::thread([]() {
			ReplacerManager::EvaluateReplacers();
		}).detach();
	}
}