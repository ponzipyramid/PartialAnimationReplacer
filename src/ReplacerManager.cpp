#include "ReplacerManager.h"

using namespace PAR;

void ReplacerManager::EvaluateReplacers()
{
	auto replacers = std::make_shared<ReplacerMap>();

	std::vector<RE::Actor*> actors{ RE::PlayerCharacter::GetSingleton() };
	RE::ProcessLists::GetSingleton()->ForEachHighActor([&actors](RE::Actor* a_actor) {
		if (a_actor->Is3DLoaded()) {
			actors.push_back(a_actor);
		}

		return RE::BSContainer::ForEachResult::kContinue;
	});

	for (const auto& actor : actors) {
		if (const auto replacer = FindReplacer(actor)) {
			replacers->insert({ actor->GetFormID(), replacer });
		}
	}
	
	replacers = _current.exchange(replacers);

	logger::info("EvaluateReplacers");
}

Replacer* ReplacerManager::FindReplacer(RE::Actor* a_actor)
{
	for (auto& replacer : _replacers) {
		if (replacer.Eval(a_actor)) {
			return &replacer;
		}
	}

	return nullptr;
}

void ReplacerManager::ApplyReplacers(RE::NiAVObject* a_playerObj)
{
	const auto replacers = _current.load();

	// apply to player
	ApplyReplacer(replacers, 0x14, a_playerObj);

	RE::NiUpdateData updateData{
		0.f,
		RE::NiUpdateData::Flag::kNone
	};

	// apply to NPCs
	RE::ProcessLists::GetSingleton()->ForEachHighActor([&replacers, &updateData](RE::Actor* a_actor) {
		const auto obj = a_actor->Get3D(false);
		if (ApplyReplacer(replacers, a_actor->GetFormID(), obj)) {
			obj->Update(updateData);
		}
		return RE::BSContainer::ForEachResult::kContinue;
	});
}

bool ReplacerManager::ApplyReplacer(const std::shared_ptr<ReplacerMap>& a_map, RE::FormID a_id, RE::NiAVObject* a_obj)
{
	const auto iter = a_map->find(a_id);
	if (iter != a_map->end()) {
		iter->second->Apply(a_obj);
		return true;
	}

	return false;
}

void ReplacerManager::Init()
{
	_current = std::make_shared<ReplacerMap>();

	logger::info("OverrideManager::Init");

	const std::string dir{ "Data\\SKSE\\PartialAnimationReplacer\\Replacers" };
	if (fs::exists(dir)) {
		for (const auto& entry : fs::directory_iterator(dir)) {
			if (!entry.is_directory()) {
				continue;
			}

			LoadDir(entry);
		}
	} else {
		logger::info("replacement dir does not exist");
	}
}

void ReplacerManager::LoadDir(const fs::directory_entry& a_dir)
{
	logger::info("Processing directory {}", a_dir.path().string());
	int found = 0;
	for (const auto& file : fs::directory_iterator(a_dir)) {
		if (file.is_directory())
			continue;

		found += (int)LoadFile(file);
	}
	logger::info("loaded {} replacer from directory {}", found, a_dir.path().string());
}

bool ReplacerManager::LoadFile(const fs::directory_entry& a_file)
{
	logger::info("Processing file {}", a_file.path().string());

	const auto ext = a_file.path().extension();

	if (ext != ".json")
		return false;

	const std::string fileName{ a_file.path().string() };

	try {
		logger::info("loading {}", fileName);

		std::ifstream f{ fileName };
		const auto data = json::parse(f);
		const auto r = data.get<ReplacerData>();

		Replacer replacer{ r };
		if (replacer.IsValid(fileName)) {
			if (_paths.count(fileName)) {
				_replacers[_paths[fileName]] = replacer;
			} else {
				_paths[fileName] = _replacers.size();
				_replacers.push_back(replacer);
			}
		} else if (_paths.count(fileName)) {
			_replacers.erase(_replacers.begin() + _paths[fileName]);
			_paths.erase(fileName);
		}

		return true;
	} catch (std::exception& e) {
		logger::info("failed to load {} - {}", fileName, e.what());

		return false;
	}
}

void ReplacerManager::LoadNodes()
{
	const std::string fileName{ "Data\\SKSE\\PartialAnimationReplacer\\Config\\arm_nodes.json" };
	std::ifstream f{ fileName };
	const auto data = json::parse(f);
	_armNodes = data.get<std::vector<std::string>>();
}

void ReplacerManager::Dump(RE::Actor* a_actor, std::string a_dir, std::string a_name)
{
	// TODO: add support for multiple frames

	// create RawReplacer and read/write to JSON
	//

	const std::string fileName{ "Data\\SKSE\\PartialAnimationReplacer\\Replacers\\" + a_dir + "\\" + a_name };

	ReplacerData data = _paths.count(fileName) ? _replacers[_paths[fileName]].GetData() : ReplacerData();

	data.frames.clear();

	Frame frame;

	if (const auto obj = a_actor->Get3D(false)) {
		for (const auto& nodeName : _armNodes) {
			if (const auto node = obj->GetObjectByName(nodeName)) {
				frame.emplace_back(Override{ nodeName, node->local.rotate });
			}
		}
	}

	data.frames.emplace_back(frame);

	json j = data;

	std::ofstream file(fileName);
	file << j;
}