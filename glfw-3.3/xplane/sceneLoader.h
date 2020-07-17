#pragma once
#include "scene.h"

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
	
};