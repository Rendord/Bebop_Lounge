#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
protected:
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	float pitch;
	float yaw;
	
public:
	Camera(glm::vec3, glm::vec3, glm::vec3, float, float);
	std::string name;
	virtual void Movement(bool[256], float) = 0;
	glm::mat4 View();
};
