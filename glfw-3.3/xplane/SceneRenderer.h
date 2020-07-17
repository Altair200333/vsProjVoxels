#pragma once

#include <vector>

#include "scene.h"
#include "window.h"
#include "shader.h"
#include <fstream>

#include "sceneLoader.h"


class SceneRenderer final
{
protected:
	const Window* window;
	Scene* scene;
	Shader shader;
public:
	
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
		int sid = scene->getSwapId(curPos, scene->demon);

		while (true)
		{
			if (!scene->inBounds(curPos))
				break;
			int id = scene->getSwapId(curPos, scene->demon);
			if (scene->voxels[id].w != 0 && sid != id)
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
		v3dStart /= float(scene->chunkSize);
		v3dEnd /= float(scene->chunkSize);
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
		int maxD = scene->demon / scene->chunkSize;
		
		while (true)
		{
			if (curPos.x < 0 || curPos.x >= maxD || curPos.y < 0 || curPos.y >= maxD || curPos.z < 0 || curPos.z >= maxD)
				return { -1,-1,-1 };
			int id = scene->getSwapChunkId(curPos);
			if (scene->voxels[id].w != 0)
			{
				glm::vec3 newStart = curPos;
				float tmp = newStart.y;
				newStart.y = newStart.z;
				newStart.z = tmp;

				float dist = glm::length(start - newStart * float(scene->chunkSize));
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
		scene->voxels.resize(scene->demon * scene->demon * scene->demon, { 0,0,0,0 });
		//importPlyVoxels("data/monu10.ply", {200, 2, 120});
		SceneLoader::importPlyModel(scene, "data/model2.ply", {50, 2, 50});

		//fillVoxels();
		
		loadAll();
	}
	bool inChunk(glm::vec3 v, glm::vec3 start)
	{
		return v.x > start.x* scene->chunkSize&& v.x<start.x * scene->chunkSize + scene->chunkSize
		&& v.y>start.y* scene->chunkSize&& v.y < start.y * scene->chunkSize + scene->chunkSize && v.z>start.z* scene->chunkSize&& v.z < start.z * scene->chunkSize + scene->chunkSize;
	}
	
	//[voxel data],..[..], [chunk data]
	//[rgb, w]..     [..], [0,0,0,w]
	void fillVoxels()
	{
		int count = scene->demon / scene->chunkSize;
		int chunks = count * count * count;

		scene->voxels.resize(scene->voxels.size() + chunks, {0,0,0,0});
		for(int i =0;i<count-1;i++)
		{
			for (int j = 0; j < count-1; j++)
			{
				for (int k = 0; k < count-1; k++)
				{
					glm::vec3 start = { i * scene->chunkSize,j * scene->chunkSize,k * scene->chunkSize };
					
					if(setChunk(start))
					{
						//printVec3(start);
						int id = scene->getChunkId({ i,j,k });
						scene->voxels[id].w = 1;
					}
				}
			}
		}
	}
	bool setChunk(glm::vec3& start)
	{
		for (int i = 0; i < scene->chunkSize; i++)
		{
			for (int j = 0; j < scene->chunkSize; j++)
			{
				for (int k = 0; k < scene->chunkSize; k++)
				{
					glm::vec3 pos = start + glm::vec3(i, j, k);
					if (scene->voxels[scene->getId(pos)].w != 0)
						return true;
				}
			}
		}
		return false;
	}
	void loadPointLights()
	{
		shader.setVec3("lights[0].pos", { 53,5,3 });
		shader.setVec3("lights[0].color", { 0.9f,0.1f,0.1f });
		
		shader.setVec3("lights[1].pos", { 20,5,57 });
		shader.setVec3("lights[1].color", { 0.1f,0.1f,0.9f });
		shader.setInt("lightCount", 2);
		
	}
	void loadAll()
	{
		shader.use();

		shader.setInt("demon", scene->demon);
		shader.setInt("chunkSize", scene->chunkSize);
		shader.setInt("chunks", scene->demon / scene->chunkSize);
		shader.setFloat("traverseDist", 100.0f);
		shader.setVec3("lightDir", normalize(glm::vec3(0.2f, -0.85f, 0.5f)));
		shader.setVec3("dirLightColor", glm::vec3(0.9f, 0.85f, 0.9f));

		loadPointLights();
		unsigned int texture1;
		glGenBuffers(1, &texture1);
		glBindBuffer(GL_TEXTURE_BUFFER, texture1);
		glBufferData(GL_TEXTURE_BUFFER, scene->voxels.size() * sizeof(glm::vec4), &scene->voxels[0], GL_STATIC_DRAW);

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
	
};
