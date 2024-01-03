#pragma once
#include <glm/glm.hpp>

namespace MatrixHelper {
	glm::mat3 yawMatrix(float degrees);
	glm::mat3 pitchMatrix(float degrees);
	glm::mat3 rollMatrix(float degrees);
	
	
}