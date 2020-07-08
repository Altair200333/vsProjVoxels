#pragma once

#include <vector>

#include "scene.h"
#include "window.h"
#include "shader.h"

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
	int demon = 500;
	double _bin_size = 1;

	struct Ray
	{
		glm::vec3 orig;
		glm::vec3 dir;

		Ray(glm::vec3 _orig, glm::vec3 _ray) : dir(_ray), orig(_orig)
		{
		};
	};

	bool intersect(Ray r, glm::vec3 min, glm::vec3 max)
	{
		float tmin = (min.x - r.orig.x) / r.dir.x;
		float tmax = (max.x - r.orig.x) / r.dir.x;

		if (tmin > tmax)
		{
			float tmp = tmin;
			tmin = tmax;
			tmax = tmp;
		}

		float tymin = (min.y - r.orig.y) / r.dir.y;
		float tymax = (max.y - r.orig.y) / r.dir.y;

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

		float tzmin = (min.z - r.orig.z) / r.dir.z;
		float tzmax = (max.z - r.orig.z) / r.dir.z;

		if (tzmin > tzmax)
		{
			float tmp = tzmin;
			tzmin = tzmax;
			tzmax = tmp;
		}

		if ((tmin > tzmax) || (tzmin > tmax))
			return false;

		if (tzmin > tmin)
			tmin = tzmin;

		if (tzmax < tmax)
			tmax = tzmax;

		return true;
	}

	//map index of int coordinates to int
	int getId(int x, int y, int z, int dim)
	{
		return z * dim * dim + y * dim + x;
	}

	int getId(glm::vec3 v, int dim)
	{
		return static_cast<int>(v.z) * dim * dim + static_cast<int>(v.y) * dim + static_cast<int>(v.x);
	}

	glm::vec3 getNext(glm::vec3 origin, glm::vec3 dir, float step)
	{
		glm::vec3 ray = glm::normalize(dir)*0.5f * step;
		return origin + ray;
	}

	glm::vec3 voxel_traversal_closest(glm::vec3 origin, glm::vec3 dir, int steps)
	{
		auto ray = origin;
		dir = glm::normalize(dir);
		for (int i=0;i<steps;i++)
		{
			ray += dir * 0.5f;
			if(voxels[getId(ray, demon)].w!=0 && intersect({origin-ray, dir}, -glm::vec3(1,1,1), glm::vec3(1, 1, 1)))
			{
				return ray;
			}
		}
		return { -1,-1,-1 };
	}

	SceneRenderer(const Window* _window, Scene* _scene) : window(_window), scene(_scene),
	                                                      shader("Shaders/vertexshader.vs", "Shaders/fragmentshader.fs",
	                                                             "Shaders/geometry.gs")
	{
		voxels.resize(demon*demon*demon, {0, 0, 0, 0});
		for(int i=0;i<10;i++)
		{
			for (int j = 0; j < 1; j++)
			{
				voxels[getId(i*3, 1, j, demon)] = { float(i)/10.0f,0.6f, 1, 1 };
			}
		}
		for (int i = 0; i < 20; i++)
		{
			voxels[getId(15, i, 15, demon)] = { 0.6f,0.6f, 1, 1 };
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

		shader.setInt("boxes", voxels.size());
	}
	void clearBuffer();

	void drawScene() const;
	void swapBuffers() const;
	~SceneRenderer() = default;
};
