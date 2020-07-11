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

	int getId(glm::vec3 v, int dim)
	{
		return static_cast<int>(v.z) * dim * dim + static_cast<int>(v.y) * dim + static_cast<int>(v.x);
	}
	int getSwapId(glm::vec3 v, int dim)
	{
		return static_cast<int>(v.y)* dim* dim + static_cast<int>(v.z)* dim + static_cast<int>(v.x);
	}
	glm::vec3 traverse(glm::vec3 v3dStart, glm::vec3 v3dEnd)
	{
		using namespace glm;
		std::swap(v3dStart.y, v3dStart.z);
		std::swap(v3dEnd.y, v3dEnd.z);
		
		const float x1 = v3dStart.x + 0.5f;
		const float y1 = v3dStart.y + 0.5f;
		const float z1 = v3dStart.z + 0.5f;
		const float x2 = v3dEnd.x + 0.5f;
		const float y2 = v3dEnd.y + 0.5f;
		const float z2 = v3dEnd.z + 0.5f;

		int i = (int)floorf(x1);
		int j = (int)floorf(y1);
		int k = (int)floorf(z1);

		const int iend = (int)floorf(x2);
		const int jend = (int)floorf(y2);
		const int kend = (int)floorf(z2);

		const int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
		const int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));
		const int dk = ((z1 < z2) ? 1 : ((z1 > z2) ? -1 : 0));

		const float deltatx = 1.0f / std::abs(x2 - x1);
		const float deltaty = 1.0f / std::abs(y2 - y1);
		const float deltatz = 1.0f / std::abs(z2 - z1);

		const float minx = floorf(x1), maxx = minx + 1.0f;
		float tx = ((x1 > x2) ? (x1 - minx) : (maxx - x1))* deltatx;
		const float miny = floorf(y1), maxy = miny + 1.0f;
		float ty = ((y1 > y2) ? (y1 - miny) : (maxy - y1))* deltaty;
		const float minz = floorf(z1), maxz = minz + 1.0f;
		float tz = ((z1 > z2) ? (z1 - minz) : (maxz - z1))* deltatz;

		vec3 curPos = { i,j,k };

		for (;;)
		{
			int id = getSwapId(curPos, demon);
			if (voxels[id].w!=0)
			{
				return curPos;
			}
			if (tx <= ty && tx <= tz)
			{
				if (i == iend) break;
				tx += deltatx;
				i += di;

				if (di == 1) curPos.x+=1;
				if (di == -1) curPos.x -= 1;
			}
			else if (ty <= tz)
			{
				if (j == jend) break;
				ty += deltaty;
				j += dj;

				if (dj == 1) curPos.y += 1;
				if (dj == -1) curPos.y -= 1;
			}
			else
			{
				if (k == kend) break;
				tz += deltatz;
				k += dk;

				if (dk == 1) curPos.z= 1;
				if (dk == -1) curPos.z -= 1;
			}
		}
		return { -1,-1,-1 };
	}
	SceneRenderer(const Window* _window, Scene* _scene) : window(_window), scene(_scene),
	                                                      shader("Shaders/vertexshader.vs", "Shaders/fragmentshader.fs",
	                                                             "Shaders/geometry.gs")
	{
		voxels = importPlyVoxels("data/castle.ply");

		for(int i=1;i<demon-1;i++)
		{
			for (int j = 1; j < demon - 1; j++)
			{
				voxels[getId(i, 1, j, demon)] = { float(i)/10.0,0.8f , 1, 1 };
			}
		}
		glm::vec3 a = traverse({ 2,10.3f,1 }, { 1,10 ,10 });
		std::swap(a.y, a.z);
		std::cout << a.x<<" "<<a.y<<" "<<a.z << "\n";
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

		shader.setInt("boxes", voxels.size());
	}
	void clearBuffer();

	void drawScene() const;
	void swapBuffers() const;
	~SceneRenderer() = default;
	std::vector<glm::vec4> importPlyVoxels(std::string filename)
	{
		std::vector<glm::vec4> data;
		data.resize(demon * demon * demon, { 0,0,0,0 });
		std::ifstream infile(filename);
		int x, y, z, r, g, b;
		while (infile >> x >> y >> z >> r >> g >> b)
		{
			int id = getId(x+20, z+2, y+20, demon);
			data[id] = { float(r)/255,float(g) / 255,float(b) / 255,1 };
		}
		return data;
	}
};
