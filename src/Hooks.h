#pragma once

class Hooks
{
	typedef bool(WINAPI* Update3DType)(RE::NiAVObject* a_obj, RE::NiUpdateData&);

public:
	static void Install();
private:
	static bool Update3D(RE::NiAVObject* a_obj, RE::NiUpdateData&);
	static inline Update3DType _Update3D;

	static inline void UpdateCharacter(RE::Actor* a_actor, float a_delta);
	inline static REL::Relocation<decltype(UpdateCharacter)> _UpdateCharacter;
};