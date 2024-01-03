#include "MatrixHelper.h"

namespace MatrixHelper {


	glm::mat3 rollMatrix(float degrees) {
		//converting degrees to radians
		float angle = degrees * 3.14159 / 180;
		
		
		glm::mat3 rollMatrix = glm::mat3(	1.0f,		0.0f,			0.0f,
											0.0f,		cos(angle), -sin(angle),
											0.0f,		sin(angle), cos(angle));
		return rollMatrix;
	}

	glm::mat3 yawMatrix(float degrees) {
		
		float angle = degrees * 3.14159 / 180;

		glm::mat3 yawMatrix = glm::mat3(	cos(angle),		-sin(angle),		0.0f,
											sin(angle),		cos(angle),		0.0f,
											0.0f,				0.0f,				1.0f);
		
		return yawMatrix;
	}

	glm::mat3 pitchMatrix(float degrees) {

		float angle = degrees * 3.14159 / 180;

		glm::mat3 pitchMatrix = glm::mat3(	cos(angle),		0.0f,		sin(angle),
											0.0,				1.0f,		0.0f,
											-sin(angle),		0.0f,		cos(angle));
		

		return pitchMatrix;
	}
}