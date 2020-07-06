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
	
	if (window.input.getKey(KeyCode::W))
		camera->cameraMove(FORWARD, deltaTime);
	if (window.input.getKey(KeyCode::S))
		camera->cameraMove(BACKWARD, deltaTime);
	if (window.input.getKey(KeyCode::A))
		camera->cameraMove(LEFT, deltaTime);
	if (window.input.getKey(KeyCode::D))
		camera->cameraMove(RIGHT, deltaTime);
	if (window.input.getKey(KeyCode::E))
		camera->cameraMove(UP, deltaTime);
	if (window.input.getKey(KeyCode::Q))
		camera->cameraMove(DOWN, deltaTime);
	
	if(window.input.getMouseButton(KeyCode::MMB))
		camera->cameraMouseLook(window.input.dx(), window.input.dy());

}