#pragma once
#include "scene.h"
#include "nlohmann/json.hpp"
class SceneLoader
{
public:
	SceneLoader() = delete;
	static void importPlyModel(Scene* scene, std::string filename, glm::vec3 shift)
	{

		std::ifstream infile(filename);

		std::string line;
		//skip all info to the end of header
		while (std::getline(infile, line))
		{
			if (line == "end_header")
				break;
		}
		int x, y, z, r, g, b;
		while (infile >> x >> y >> z >> r >> g >> b)
		{
			//flip x to match model in magicaVoxel
			x = -x;
			if (scene->inBounds({ x + shift.x, z + shift.y, y + shift.z }))
			{
				int mat = 3;
				if (r == 0 && g == 0 && b == 0)
					mat = 2;
				if (r == 255 && g == 255 && b == 255)
					mat = 4;
				int id = scene->getId(x + shift.x, z + shift.y, y + shift.z, scene->demon);
				scene->voxels[id] = { float(r) / 255,float(g) / 255,float(b) / 255, mat };
			}
		}
	}
	static void loadFromJson(Scene* scene, std::string path)
	{
		using json = nlohmann::json;
		std::ifstream i(path);
		
		json j;
		i >> j;

		if (j.find("demon") != j.end())
			scene->demon = j.at("demon");
		if (j.find("chunkSize") != j.end())
			scene->chunkSize = j.at("chunkSize");
		if (j.find("models") != j.end())
			for (auto& [key, value] : j.at("models").items())
			{
				importPlyModel(scene, value.at("name"), { value.at("shift")[0],value.at("shift")[1] ,value.at("shift")[2] });
			}
	}
};