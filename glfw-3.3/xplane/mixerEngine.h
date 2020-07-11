#pragma once
#include <iomanip>

#include "scene.h"
#include "SceneRenderer.h"

#include "window.h"
#include "Object.h"

class MixerEngine final
{
public:
	Window window;

	Scene scene;
	SceneRenderer viewportRenderer;
    
	MixerEngine(int width, int height):scene(&window), window(width, height, "Voxel ray"), viewportRenderer(&window, &scene)
	{
		window.hideCursor();

	}

	void mainLoop();

	void onUpdate();

	~MixerEngine() = default;
protected:
	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	void calcDeltaTime(double currentFrame)
	{
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

};
