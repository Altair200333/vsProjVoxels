#pragma once

#include <vector>

#include "scene.h"
#include "window.h"
#include "shader.h"
#include <fstream>


class SceneRenderer final
{
protected:
	const Window* window;
	Scene* scene;
	Shader shader;
public:
	std::vector<glm::vec4> voxels;

	const int demon = 100;
	const int chunkSize = 5;
	//map index of int coordinates to int
	int getId(int x, int y, int z, int dim)
	{
		return z * dim * dim + y * dim + x;
	}
	int getChunkId(glm::vec3 pos)
	{
		int chunks = demon / chunkSize;
		return demon*demon*demon + pos.z * chunks * chunks + pos.y * chunks + pos.x;
	}
	int getSwapChunkId(glm::vec3 pos)
	{
		int chunks = demon / chunkSize;
		return demon * demon * demon + pos.y * chunks * chunks + pos.z * chunks + pos.x;
	}
	int getId(glm::vec3 v)
	{
		return round(v.z) * demon * demon + round(v.y) * demon + round(v.x);
	}
	int getSwapId(glm::vec3 v, int dim)
	{
		return static_cast<int>(v.y)* dim* dim + static_cast<int>(v.z)* dim + static_cast<int>(v.x);
	}
	void printVec3(glm::vec3 v)
	{
		std::cout << v.x<<" "<<v.y<<" "<<v.z<<"\n";
	}
	bool intersect(glm::vec3 orig, glm::vec3 dir, glm::vec3 min, glm::vec3 max)
	{
		float tmin = (min.x - orig.x) / dir.x;
		float tmax = (max.x - orig.x) / dir.x;

		if (tmin > tmax)
		{
			float tmp = tmin;
			tmin = tmax;
			tmax = tmp;
		}

		float tymin = (min.y - orig.y) / dir.y;
		float tymax = (max.y - orig.y) / dir.y;

		if (tymin > tymax)
		{
			float tmp = tymin;
			tymin = tymax;
			tymax = tmp;
		}

		if ((tmin > tymax) || (tymin > tmax))
			return false;

		if (tymin > tmin)
			tmin = tymin;

		if (tymax < tmax)
			tmax = tymax;

		float tzmin = (min.z - orig.z) / dir.z;
		float tzmax = (max.z - orig.z) / dir.z;

		if (tzmin > tzmax)
		{
			float tmp = tzmin;
			tzmin = tzmax;
			tzmax = tmp;
		}

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		return true;
	}
	struct pointLight
	{
		glm::vec3 pos;
		glm::vec3 color;
	};
	glm::vec3 traverse2(glm::vec3 v3dStart, glm::vec3 v3dEnd)
	{
		float tmp = v3dStart.y;
		v3dStart.y = v3dStart.z;
		v3dStart.z = tmp;
		tmp = v3dEnd.y;
		v3dEnd.y = v3dEnd.z;
		v3dEnd.z = tmp;

		float x1 = v3dStart.x + 0.5f;
		float y1 = v3dStart.y + 0.5f;
		float z1 = v3dStart.z + 0.5f;
		float x2 = v3dEnd.x + 0.5f;
		float y2 = v3dEnd.y + 0.5f;
		float z2 = v3dEnd.z + 0.5f;

		int i = int(floor(x1));
		int j = int(floor(y1));
		int k = int(floor(z1));

		int iend = int(floor(x2));
		int jend = int(floor(y2));
		int kend = int(floor(z2));

		int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
		int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));
		int dk = ((z1 < z2) ? 1 : ((z1 > z2) ? -1 : 0));

		float deltatx = 1.0f / abs(x2 - x1);
		float deltaty = 1.0f / abs(y2 - y1);
		float deltatz = 1.0f / abs(z2 - z1);

		float minx = floor(x1), maxx = minx + 1.0f;
		float tx = ((x1 > x2) ? (x1 - minx) : (maxx - x1))* deltatx;
		float miny = floor(y1), maxy = miny + 1.0f;
		float ty = ((y1 > y2) ? (y1 - miny) : (maxy - y1))* deltaty;
		float minz = floor(z1), maxz = minz + 1.0f;
		float tz = ((z1 > z2) ? (z1 - minz) : (maxz - z1))* deltatz;

		glm::vec3 curPos = glm::vec3(i, j, k);
		int sid = getSwapId(curPos, demon);

		while (true)
		{
			if (!inBounds(curPos))
				break;
			int id = getSwapId(curPos, demon);
			if (voxels[id].w != 0 && sid != id)
			{
				float tmp = curPos.y;
				curPos.y = curPos.z;
				curPos.z = tmp;
				return curPos;
			}
			if (tx <= ty && tx <= tz)
			{
				if (i == iend) break;
				tx += deltatx;
				i += di;

				if (di == 1) curPos.x += 1;
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

				if (dk == 1) curPos.z += 1;
				if (dk == -1) curPos.z -= 1;
			}
		}
		return glm::vec3(-1, -1, -1);
	}
	glm::vec3 traverseChunk(glm::vec3 v3dStart, glm::vec3 v3dEnd)
	{
		glm::vec3 start = v3dStart;
		glm::vec3 dir = glm::normalize(v3dEnd - v3dStart);
		v3dStart /= float(chunkSize);
		v3dEnd /= float(chunkSize);
		float tmp = v3dStart.y;
		v3dStart.y = v3dStart.z;
		v3dStart.z = tmp;
		tmp = v3dEnd.y;
		v3dEnd.y = v3dEnd.z;
		v3dEnd.z = tmp;

		float x1 = v3dStart.x + 0.5f;
		float y1 = v3dStart.y + 0.5f;
		float z1 = v3dStart.z + 0.5f;
		float x2 = v3dEnd.x + 0.5f;
		float y2 = v3dEnd.y + 0.5f;
		float z2 = v3dEnd.z + 0.5f;

		int i = int(floor(x1));
		int j = int(floor(y1));
		int k = int(floor(z1));

		int iend = int(floor(x2));
		int jend = int(floor(y2));
		int kend = int(floor(z2));

		int di = ((x1 < x2) ? 1 : ((x1 > x2) ? -1 : 0));
		int dj = ((y1 < y2) ? 1 : ((y1 > y2) ? -1 : 0));
		int dk = ((z1 < z2) ? 1 : ((z1 > z2) ? -1 : 0));

		float deltatx = 1.0f / abs(x2 - x1);
		float deltaty = 1.0f / abs(y2 - y1);
		float deltatz = 1.0f / abs(z2 - z1);

		float minx = floor(x1), maxx = minx + 1.0f;
		float tx = ((x1 > x2) ? (x1 - minx) : (maxx - x1))* deltatx;
		float miny = floor(y1), maxy = miny + 1.0f;
		float ty = ((y1 > y2) ? (y1 - miny) : (maxy - y1))* deltaty;
		float minz = floor(z1), maxz = minz + 1.0f;
		float tz = ((z1 > z2) ? (z1 - minz) : (maxz - z1))* deltatz;

		glm::vec3 curPos = glm::vec3(i, j, k);
		int maxD = demon / chunkSize;
		
		while (true)
		{
			if (curPos.x < 0 || curPos.x >= maxD || curPos.y < 0 || curPos.y >= maxD || curPos.z < 0 || curPos.z >= maxD)
				return { -1,-1,-1 };
			int id = getSwapChunkId(curPos);
			if (voxels[id].w != 0)
			{
				glm::vec3 newStart = curPos;
				float tmp = newStart.y;
				newStart.y = newStart.z;
				newStart.z = tmp;

				float dist = glm::length(start - newStart * float(chunkSize));
				auto a = traverse2(start + dir * dist * 0.9f, start + dir * dist * 1.1f);
				if (a.x != -1)
					return a;
			}
			if (tx <= ty && tx <= tz)
			{
				if (i == iend) break;
				tx += deltatx;
				i += di;

				if (di == 1) curPos.x += 1;
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

				if (dk == 1) curPos.z += 1;
				if (dk == -1) curPos.z -= 1;
			}
		}
		return glm::vec3(-1, -1, -1);
	}
	// 2- reflective
	// 3- diffuse colored
	// 4- glass
	SceneRenderer(const Window* _window, Scene* _scene) : window(_window), scene(_scene),
	                                                      shader("Shaders/vertexshader.vs", "Shaders/fragmentshader.fs",
	                                                             "Shaders/geometry.gs")
	{
		voxels.resize(demon * demon * demon, { 0,0,0,0 });
		//importPlyVoxels("data/monu10.ply", {200, 2, 120});
		importPlyVoxels("data/model2.ply", {50, 2, 50});

		//fillVoxels();
		
		loadAll();
	}
	bool inChunk(glm::vec3 v, glm::vec3 start)
	{
		return v.x > start.x* chunkSize&& v.x<start.x * chunkSize + chunkSize && v.y>start.y* chunkSize&& v.y < start.y * chunkSize + chunkSize && v.z>start.z* chunkSize&& v.z < start.z * chunkSize + chunkSize;
	}
	
	//[voxel data],..[..], [chunk data]
	//[rgb, w]..     [..], [0,0,0,w]
	void fillVoxels()
	{
		int count = demon / chunkSize;
		int chunks = count * count * count;

		voxels.resize(voxels.size() + chunks, {0,0,0,0});
		for(int i =0;i<count-1;i++)
		{
			for (int j = 0; j < count-1; j++)
			{
				for (int k = 0; k < count-1; k++)
				{
					glm::vec3 start = { i * chunkSize,j * chunkSize,k * chunkSize };
					
					if(setChunk(start))
					{
						//printVec3(start);
						int id = getChunkId({ i,j,k });
						voxels[id].w = 1;
					}
				}
			}
		}
	}
	bool setChunk(glm::vec3& start)
	{
		for (int i = 0; i < chunkSize; i++)
		{
			for (int j = 0; j < chunkSize; j++)
			{
				for (int k = 0; k < chunkSize; k++)
				{
					glm::vec3 pos = start + glm::vec3(i, j, k);
					if (voxels[getId(pos)].w != 0)
						return true;
				}
			}
		}
		return false;
	}
	void loadPointLights()
	{
		shader.setVec3("lights[0].pos", { 10,5,10 });
		shader.setVec3("lights[0].color", { 0.9f,0.1f,0.1f });
		shader.setInt("lightCount", 1);
	}
	void loadAll()
	{
		shader.use();

		shader.setInt("demon", demon);
		shader.setInt("chunkSize", chunkSize);
		shader.setInt("chunks", demon / chunkSize);
		shader.setFloat("traverseDist", 100.0f);
		shader.setVec3("lightDir", normalize(glm::vec3(0.2f, -0.85f, 0.5f)));
		shader.setVec3("dirLightColor", glm::vec3(0.9f, 0.85f, 0.9f));

		loadPointLights();
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
		
		std::string line;
		//skip all info to the end of header
		while (std::getline(infile, line))
		{
			if(line=="end_header")
				break;
		}
		int x, y, z, r, g, b;
		while (infile >> x >> y >> z >> r >> g >> b)
		{
			//flip x to match model in magicaVoxel
			x = -x;
			if (inBounds({ x + shift.x, z + shift.y, y + shift.z }))
			{
				int mat = 3;
				if (r == 0 && g == 0 && b == 0)
					mat = 2;
				if (r == 255 && g == 255 && b == 255)
					mat = 4;
				int id = getId(x + shift.x, z + shift.y, y + shift.z, demon);
				voxels[id] = { float(r) / 255,float(g) / 255,float(b) / 255, mat };
			}
		}
	}
};
