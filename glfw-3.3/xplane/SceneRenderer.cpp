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
	//ooo kurwa magic
    GLuint vao; glGenVertexArrays(1, &vao); glBindVertexArray(vao); glDrawArrays(GL_POINTS, 0, 1);
}
void SceneRenderer::swapBuffers() const
{
    window->swapBuffers();
}