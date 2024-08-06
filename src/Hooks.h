#pragma once

class Hooks
{
	typedef bool(WINAPI* Update3DType)(RE::Actor* a_actor);
public:
	static void Install();
private:
	static bool Update3D(RE::Actor* a_actor);
	static inline Update3DType _Update3D;
};