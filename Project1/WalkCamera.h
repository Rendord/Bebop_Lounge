#pragma once
#include "Camera.h"

class WalkCamera : public Camera {
private:
	bool falling;
	bool jumping;
	int jump_counter;
	const int MAX_JUMP = 50;
	const int JUMP_MULTIPLIER = 5;
	const float FALLING_MULTIPLIER = 1.0f / (float)JUMP_MULTIPLIER;
	
public:
	WalkCamera(glm::vec3, glm::vec3, glm::vec3, float, float);
	void Movement(bool[256], float);
};