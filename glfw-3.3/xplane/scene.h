#pragma once
#include <vector>

#include "camera.h"

#include "Object.h"
#include "window.h"
//Scene class is more of a container that has information about objects, materials, lighting, cameras etc
class Scene final
{
public:
	//voxel data of map stored like this - [r, g, b, w] where w is material identifier
	std::vector<glm::vec4> voxels;

	//dimension of squared world
	int demon = 100;
	
	//size of chunk batched
	int chunkSize = 5;

	//--
	Object* camera;
	std::string sceneName = "Untitled.mxr";
	
	Window* window;
	Scene(Window* _window):window(_window)
	{
		Object* object = new Object();
		object->addComponent(new Transform(object, {20,10,20}));
		object->addComponent(new Camera(object, static_cast<float>(window->width) / window->height, 60.0f));
		addCamera(object);
	}

	void addCamera(Object* object);
	
	Camera* getActiveCamera();
	~Scene();
	void clear();

	Scene& operator=(Scene& other) = delete;

	//map index of int coordinates to int
	int getId(int x, int y, int z, int dim)
	{
		return z * dim * dim + y * dim + x;
	}
	int getChunkId(glm::vec3 pos)
	{
		int chunks = demon / chunkSize;
		return demon * demon * demon + pos.z * chunks * chunks + pos.y * chunks + pos.x;
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
	bool inBounds(glm::vec3 point)
	{
		return point.x >= 0 && point.x < demon && point.y >= 0 && point.y < demon && point.z >= 0 && point.z < demon;
	}
};