#pragma once

namespace PAR
{
	class Hooks
	{
	public:
		static void Install();

	private:
		static void UpdatePlayer(RE::Actor* a_actor, float a_delta);
		static inline REL::Relocation<decltype(UpdatePlayer)> _UpdatePlayer;

		static inline float _lastUpdated;
	};
}
