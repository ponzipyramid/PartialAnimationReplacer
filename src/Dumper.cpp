#include "Dumper.h"

using namespace PAR;

bool Dumper::QueueDump(RE::Actor* a_actor, std::string a_dir, std::string a_name, std::string a_nodes, int a_target, bool a_rotate, bool a_translate, bool a_scale)
{
	std::unique_lock lock{ _mutex };

	std::string id{ a_dir + "\\" + a_name };

	const auto iter = _jobs.find(id);
	if (iter != _jobs.end()) {
		iter->second.Complete();
	}

	const std::string fileName{ std::format("Data\\SKSE\\PartialAnimationReplacer\\Config\\{}\\{}", a_dir, a_nodes) };

	if (!fs::exists(fileName))
		return false;

	std::ifstream f{ fileName };
	const auto data = json::parse(f);
	const auto nodes = data.get<std::vector<std::string>>();

	if (nodes.empty())
		return false;
	
	_jobs.insert({ id, DumpJob{ a_actor, a_dir, a_name, nodes, a_target, a_rotate, a_translate, a_scale } });

	return true;
}

void Dumper::OnFrame()
{
	if (_mutex.try_lock()) {
		std::vector<std::string> clear;

		for (auto& [id, job] : _jobs) {
			if (job.Record()) {
				logger::info("completing job {}", id);
				job.Complete();
				clear.push_back(id);
			}
		}

		for (const auto& id : clear) {
			_jobs.erase(id);
		}

		_mutex.unlock();
	}
}