#include "SceneRenderer.h"

#include <corecrt_math_defines.h>


#include "MeshRenderer.h"
#include <vector>


void SceneRenderer::clearBuffer()
{
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneRenderer::drawScene() const
{
    auto camera = scene->getActiveCamera();
    
    shader.use();

    const float closeHeight = 2 * tan(camera->zoom / 2 * M_PI / 180);
    const float scale = closeHeight / window->height;
	
    shader.setVec3("camera.position", camera->owner->getComponent<Transform>()->position);
    shader.setFloat("camera.scale", scale);
    shader.setVec3("camera.front", camera->Front);
    shader.setVec3("camera.right", camera->Right);
    shader.setVec3("camera.up", camera->Up);
    shader.setInt("camera.width", window->width);
    shader.setInt("camera.height", window->height);
    //
    shader.setInt("demon", demon);
	
    GLuint vao; glGenVertexArrays(1, &vao); glBindVertexArray(vao); glDrawArrays(GL_POINTS, 0, 1);
}
void SceneRenderer::swapBuffers() const
{
    window->swapBuffers();
}