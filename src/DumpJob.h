#pragma once

#include "Replacer.h"

namespace PAR
{
	class DumpJob
	{
	public:
		DumpJob(RE::Actor* a_actor, std::string a_dir, std::string a_name, std::vector<std::string> a_nodes, int a_target, bool a_rotate, bool a_translate, bool a_scale) :
			_actor(a_actor), _dir(a_dir), _name(a_name), _nodes(a_nodes), _target(a_target), _rotate(a_rotate), _translate(a_translate), _scale(a_scale) {}
		inline bool IsDone() const { return _frames.size() >= _target; }
		inline bool Record()
		{
			if (IsDone())
				return true;
			
			if (const auto obj = _actor->Get3D(false)) {
				Frame frame;

				for (const auto& nodeName : _nodes) {
					if (const auto node = obj->GetObjectByName(nodeName)) {
						frame.emplace_back(Override{ nodeName, node->local });
					}
				}

				if (!frame.empty())
					_frames.emplace_back(frame);
			}

			return IsDone();
		}

		inline void Complete() const { 
			ReplacerData data;

			const std::string fileName{ "Data\\SKSE\\PartialAnimationReplacer\\Replacers\\" + _dir + "\\" + _name };
			const fs::directory_entry entry{ fileName };
			if (entry.exists()) {
				try {
					std::ifstream f{ fileName };
					json d;
					const auto existingData = json::parse(f);
					data = existingData.get<ReplacerData>();
				} catch (...) {}
			}

			data.frames = _frames;
			data.rotate = _rotate;
			data.translate = _translate;
			data.scale = _scale;

			json j = data;

			std::string s = j.dump(2);
			std::ofstream file(fileName);
			file << std::setfill(' ') << std::setw(2) << j;
		}
	private:
		RE::Actor* _actor;
		
		std::string _dir;
		std::string _name;

		std::vector<std::string> _nodes;
		std::vector<Frame> _frames;

		bool _rotate;
		bool _translate;
		bool _scale;

		int _target;
	};
}