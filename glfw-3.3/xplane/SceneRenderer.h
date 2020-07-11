#pragma once

#include <vector>

#include "scene.h"
#include "window.h"
#include "shader.h"
#include <fstream>
#define HIT		 1		// Ray hit primitive
#define MISS	 0		// Ray missed primitive
#define INPRIM	-1		// Ray started inside primitive

class SceneRenderer final
{
protected:
	const Window* window;
	Scene* scene;
	Shader shader;
public:
	std::vector<glm::vec4> voxels;
	int demon = 200;

	//map index of int coordinates to int
	int getId(int x, int y, int z, int dim)
	{
		return z * dim * dim + y * dim + x;
	}

	int getId(glm::vec3 v)
	{
		return round(v.z) * demon * demon + round(v.y) * demon + round(v.x);
	}
	int getSwapId(glm::vec3 v, int dim)
	{
		return static_cast<int>(v.y)* dim* dim + static_cast<int>(v.z)* dim + static_cast<int>(v.x);
	}
	
	SceneRenderer(const Window* _window, Scene* _scene) : window(_window), scene(_scene),
	                                                      shader("Shaders/vertexshader.vs", "Shaders/fragmentshader.fs",
	                                                             "Shaders/geometry.gs")
	{
		voxels.resize(demon * demon * demon, { 0,0,0,0 });
		importPlyVoxels("data/monu10.ply", {130, 2, 120});
		importPlyVoxels("data/monu1.ply", {80, 2, 10});

		for(int i=1;i<demon-1;i++)
		{
			for (int j = 1; j < demon - 1; j++)
			{
				voxels[getId(i, 1, j, demon)] = { 0.5,0.8f , 1, 2 };
			}
		}
		
		loadAll();
	}
	void loadAll()
	{
		unsigned int texture1;
		glGenBuffers(1, &texture1);
		glBindBuffer(GL_TEXTURE_BUFFER, texture1);
		glBufferData(GL_TEXTURE_BUFFER, voxels.size() * sizeof(glm::vec4), &voxels[0], GL_STATIC_DRAW);

		GLuint id;
		glGenTextures(1, &id);
		glBindBuffer(GL_TEXTURE_BUFFER, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, texture1);

		glBindBuffer(GL_TEXTURE_BUFFER, 0);
		glBindTexture(GL_TEXTURE_BUFFER, 0);

		shader.setInt("demon", demon);
	}
	void clearBuffer();

	void drawScene() const;
	void swapBuffers() const;
	~SceneRenderer() = default;
	
	bool inBounds(glm::vec3 point)
	{
		return point.x >= 0 && point.x < demon && point.y >= 0 && point.y < demon && point.z >= 0 && point.z < demon;
	}
	void importPlyVoxels(std::string filename, glm::vec3 shift)
	{
		
		std::ifstream infile(filename);
		int x, y, z, r, g, b;
		while (infile >> x >> y >> z >> r >> g >> b)
		{
			if (inBounds({ x + shift.x, z + shift.y, y + shift.z }))
			{
				int id = getId(x + shift.x, z + shift.y, y + shift.z, demon);
				voxels[id] = { float(r) / 255,float(g) / 255,float(b) / 255,1 };
			}
		}
	}
};
