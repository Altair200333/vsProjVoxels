#include "scene.h"



void Scene::addCamera(Object* object)
{
	camera = object;
}

Camera* Scene::getActiveCamera()
{
	return camera->getComponent<Camera>();
}

Scene::~Scene()
{
	clear();
}

void Scene::clear()
{
	delete camera;
}
