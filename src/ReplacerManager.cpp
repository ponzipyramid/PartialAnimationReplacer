#include "ReplacerManager.h"

using namespace PAR;

void ReplacerManager::EvaluateReplacers()
{
	auto replacers = std::make_shared<ReplacerMap>();

	std::unique_lock lock{ _mutex };

	std::vector<RE::Actor*> actors{ RE::PlayerCharacter::GetSingleton() };
	RE::ProcessLists::GetSingleton()->ForEachHighActor([&actors](RE::Actor* a_actor) {
		if (a_actor->Is3DLoaded()) {
			actors.emplace_back(a_actor);
		}

		return RE::BSContainer::ForEachResult::kContinue;
	});

	for (const auto& actor : actors) {
		if (const auto replacer = FindReplacer(actor)) {
			replacers->insert({ actor->GetFormID(), replacer });
		}
	}
	
	replacers = _current.exchange(replacers);
}

std::shared_ptr<Replacer> ReplacerManager::FindReplacer(RE::Actor* a_actor)
{
	for (auto& replacer : _replacers) {
		if (replacer->Eval(a_actor)) {
			return replacer;
		}
	}

	return nullptr;
}

void ReplacerManager::ApplyReplacers(RE::NiAVObject* a_playerObj)
{
	if (!_enabled)
		return;

	const auto replacers = _current.load();

	// apply to player
	ApplyReplacer(replacers, 0x14, a_playerObj);

	RE::NiUpdateData updateData{
		0.f,
		RE::NiUpdateData::Flag::kNone
	};

	// apply to NPCs
	RE::ProcessLists::GetSingleton()->ForEachHighActor([&replacers, &updateData](RE::Actor* a_actor) {
		if (const auto obj = a_actor->Get3D(false)) {
			if (ApplyReplacer(replacers, a_actor->GetFormID(), obj)) {
				obj->Update(updateData);
			}
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

	logger::info("ReplacerManager::Init");

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

bool ReplacerManager::ReloadFile(const fs::directory_entry& a_file)
{
	std::unique_lock lock{ _mutex };  // prevent read/writes from replacers
	
	// invalidate current replacers
	auto replacers = std::make_shared<ReplacerMap>();
	replacers = _current.exchange(replacers);

	return LoadFile(a_file);
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
		const auto replacer = std::make_shared<Replacer>(data.get<ReplacerData>());

		if (replacer->IsValid(fileName)) {
			if (_paths.count(fileName)) {
				_replacers[_paths[fileName]] = replacer;
			} else {
				_paths[fileName] = _replacers.size();
				_replacers.emplace_back(replacer);
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
