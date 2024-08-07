#include "OverrideManager.h"

void OverrideManager::Init()

{
	logger::info("OverrideManager::Init");

	std::ifstream f("Data\\SKSE\\NodeManipulator\\output.json");
	json data = json::parse(f);

	for (auto& [key, val] : data.items()) {
		const auto nodeName = val["node_name"].template get<std::string>();

		const auto x = val["x"].template get<float>();
		const auto y = val["y"].template get<float>();
		const auto z = val["z"].template get<float>();

		const RE::NiMatrix3 rot{ x, y, z };

		const Override nodeOverride{ nodeName, rot };
		overrides.push_back(nodeOverride);
	}
}

void OverrideManager::ApplyOverrides(RE::NiAVObject* a_obj) {
	if (const auto ref = a_obj->GetUserData()) {
		if (const auto actor = ref->As<RE::Actor>()) {

		}
	}
}