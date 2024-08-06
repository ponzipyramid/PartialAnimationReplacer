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

		//logger::info("{} - [{}, {}, {}] [{}, {}, {}] [{}, {}, {}]", nodeName, rot.entry[0][0], rot.entry[0][1], rot.entry[0][2], rot.entry[1][0], rot.entry[1][1], rot.entry[1][2], rot.entry[2][0], rot.entry[2][1], rot.entry[2][2]);

		logger::info("Loaded override: {} = ({}, {}, {})", nodeName, x, y, z);

		const Override nodeOverride{ nodeName, rot };
		overrides.push_back(nodeOverride);
	}
}