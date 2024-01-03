#pragma once
#include "Camera.h"

class DroneCamera : public Camera {
public:
	DroneCamera(glm::vec3, glm::vec3, glm::vec3, float, float);
	void Movement(bool[256], float);
};