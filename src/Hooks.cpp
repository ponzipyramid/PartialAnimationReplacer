#include "Hooks.h"

void ApplyOffsets(RE::NiAVObject* a_obj)
{
	const RE::NiMatrix3 rot{};
	if (const auto node = a_obj->GetObjectByName("NPC L UpperArm [LUar]"))
		node->local.rotate = rot;
}

void UpdateArmOffsets(RE::NiAVObject* a_obj, RE::NiUpdateData* a_update)
{
	ApplyOffsets(a_obj);

	RE::ProcessLists::GetSingleton()->ForEachHighActor([a_update](RE::Actor* a_actor) {
		if (const auto obj = a_actor->Get3D(false)) {
			ApplyOffsets(obj);
			RE::NiUpdateData updateData{
				0.f,
				RE::NiUpdateData::Flag::kDirty
			};

			obj->Update(updateData);
		}

		return RE::BSContainer::ForEachResult::kContinue;
	});
}

struct UpdateThirdPerson
{
	static void thunk(RE::NiAVObject* a_obj, RE::NiUpdateData* updateData)
	{
		bool mapMenu = RE::UI::GetSingleton()->IsMenuOpen("MapMenu");

		if (mapMenu) {
			func(a_obj, updateData);
			return;
		}

		UpdateArmOffsets(a_obj, updateData);
		func(a_obj, updateData);
	}
	static inline REL::Relocation<decltype(thunk)> func;
	static inline constexpr std::size_t size{ 5 };
};

void Hooks::Install()
{
	stl::write_thunk_call<UpdateThirdPerson>(REL::RelocationID(39446, 40522).address() + 0x94);
	logger::info("Hooks installed");
}