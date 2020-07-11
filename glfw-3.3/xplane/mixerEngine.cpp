#include "mixerEngine.h"
#include "Loger.h"

void MixerEngine::mainLoop()
{
	while (!window.shouldClose())
	{
		calcDeltaTime(window.getTime());
		
		viewportRenderer.clearBuffer();
		viewportRenderer.drawScene();
		viewportRenderer.swapBuffers();

		window.pollEvents();
		onUpdate();
	}
}

void MixerEngine::onUpdate()
{
	
	if (window.input.getKeyUp(KeyCode::ESC))
		glfwSetWindowShouldClose(window.window, true);
	auto camera = scene.getActiveCamera();

	if (camera == nullptr)
		return;

	glm::vec3 direction = {0,0,0};
	if (window.input.getKey(KeyCode::W))
		direction += camera->Front;
	if (window.input.getKey(KeyCode::S))
		direction -= camera->Front;
	if (window.input.getKey(KeyCode::A))
		direction -= camera->Right;
	if (window.input.getKey(KeyCode::D))
		direction += camera->Right;
	if (window.input.getKey(KeyCode::E))
		direction += camera->Up;
	if (window.input.getKey(KeyCode::Q))
		direction -= camera->Up;

	glm::vec3& postion = camera->owner->getComponent<Transform>()->position;
	glm::vec3 newPos = postion + direction * deltaTime*camera->movementSpeed;
	
	if(viewportRenderer.voxels[viewportRenderer.getId(newPos)].w ==0 && viewportRenderer.inBounds(newPos))
		camera->cameraMove(direction, deltaTime);
	
	if(window.input.getMouseButton(KeyCode::MMB))
		camera->cameraMouseLook(window.input.dx(), window.input.dy());

}