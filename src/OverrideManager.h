#pragma once

struct Override
{
	std::string nodeName;
	RE::NiMatrix3 rot;
};

class OverrideManager
{
public:
	static void Init();
	static void ApplyOverrides(RE::NiAVObject* a_obj);
private:
	static inline std::vector<Override> overrides;
};