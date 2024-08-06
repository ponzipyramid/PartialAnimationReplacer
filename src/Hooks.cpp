#include "Hooks.h"

void UpdateArmOffsets(RE::NiAVObject* a_obj, RE::NiUpdateData* a_update)
{
	const RE::NiMatrix3 rot{};
	a_obj->GetObjectByName("NPC L UpperArm [LUar]")->local.rotate = rot;

	RE::ProcessLists::GetSingleton()->ForEachHighActor([a_update, rot](RE::Actor* a_actor) {
		if (const auto obj = a_actor->Get3D(false)) {
			if (const auto node = obj->GetObjectByName("NPC L UpperArm [LUar]")) {
				node->local.rotate = rot;

				RE::NiUpdateData updateData{
					0.f,
					RE::NiUpdateData::Flag::kDirty
				};

				obj->Update(updateData);
			}
		}

		return RE::BSContainer::ForEachResult::kContinue;
	});
}

struct UpdateFirstPerson
{
	static void thunk(RE::NiAVObject* firstpersonObject, RE::NiUpdateData* updateData)
	{
		auto camera = RE::PlayerCamera::GetSingleton();
		bool mapMenu = RE::UI::GetSingleton()->IsMenuOpen("MapMenu");

		if (camera->IsInFreeCameraMode() || mapMenu) {
			func(firstpersonObject, updateData);
			return;
		}

		UpdateArmOffsets(firstpersonObject, updateData);
		func(firstpersonObject, updateData);
	}
	static inline REL::Relocation<decltype(thunk)> func;
	static inline constexpr std::size_t size{ 5 };
};

void Hooks::Install()
{
	stl::write_thunk_call<UpdateFirstPerson>(REL::RelocationID(39446, 40522).address() + 0x94);
	logger::info("Hooks installed");
}