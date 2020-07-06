#pragma once
#include "camera.h"

#include "Object.h"
#include "window.h"
//Scene class is more of a container that has information about objects, materials, lighting, cameras etc
class Scene final
{
public:
	
	Object* camera;
	std::string sceneName = "Untitled.mxr";
	
	Window* window;
	Scene(Window* _window):window(_window)
	{
		Object* object = new Object();
		object->addComponent(new Transform(object));
		object->addComponent(new Camera(object, static_cast<float>(window->width) / window->height, 60.0f));
		addCamera(object);
	}

	void addCamera(Object* object);
	
	Camera* getActiveCamera();
	~Scene();
	void clear();

	Scene& operator=(Scene& other) = delete;

};