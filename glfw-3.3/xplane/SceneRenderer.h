#pragma once

#include "scene.h"
#include "window.h"
#include "shader.h"

class SceneRenderer final
{
protected:
    const Window* window;
    Scene* scene;
    Shader shader;
public:
   
    SceneRenderer(const Window* _window, Scene* _scene) :window(_window), scene(_scene),
		shader("Shaders/vertexshader.vs", "Shaders/fragmentshader.fs", "Shaders/geometry.gs")
    {}
	
    void clearBuffer();

    void drawScene() const;
    void swapBuffers() const;
    ~SceneRenderer() = default;
};
