#pragma once
#include <iomanip>

#include "scene.h"
#include "SceneRenderer.h"

#include "window.h"
#include "Object.h"
static float cubedata[] = {
        -1.0f, -1.0f, -1.0f, // triangle 1 : begin
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 0.0f, 0.0f,       // triangle 1 : end
        1.0f, 1.0f, -1.0f, // triangle 2 : begin
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        0.0f, 0.0f, -1.0f, // triangle 2 : end
        1.0f, -1.0f, 1.0f,//3
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        0.0f, -1.0f, 0.0f,
        1.0f, 1.0f, -1.0f,//4
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,//5
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,//6
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        0.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,//7
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,//8
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,//9
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,//10
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,//11
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,//12
        -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        0.0f, 0.0f, 1.0f
};
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
