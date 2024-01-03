#include "Camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float yaw, float pitch)
{
	cameraPos = pos;
	cameraFront = front;
	cameraUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
}

glm::mat4 Camera::View() {
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}


