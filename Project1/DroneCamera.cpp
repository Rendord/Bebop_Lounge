#include "DroneCamera.h"

DroneCamera::DroneCamera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float yaw, float pitch) : Camera(pos, front, up, yaw, pitch)
{
}

void DroneCamera::Movement(bool keybuffer[256], float deltatime) {
	float speed = 0.01f * deltatime;

	if (keybuffer[119]) //w
		cameraPos += speed * cameraFront;
	if (keybuffer[97]) //a
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
	if (keybuffer[115]) //s
		cameraPos -= speed * cameraFront;
	if (keybuffer[100]) //d
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
	if (keybuffer[101])
		cameraPos += cameraUp * speed;
	if (keybuffer[113])
		cameraPos -= cameraUp * speed;

	// looking around


	if (keybuffer[106]) { //j
		yaw -= speed * 4;
		//glm::mat3 yawMatrix = MatrixHelper::yawMatrix(-speed * 5);
		////cameraPos = yawMatrix * cameraPos;
		//cameraFront = yawMatrix * cameraFront;
		//cameraUp = yawMatrix * cameraUp;
	}

	if (keybuffer[108]) { //l
		yaw += speed * 4;
		//glm::mat3 yawMatrix = MatrixHelper::yawMatrix(speed * 5);
		////cameraPos = yawMatrix * cameraPos;
		//cameraFront = yawMatrix * cameraFront;
		//cameraUp = yawMatrix * cameraUp;
	}


	if (keybuffer[105]) { //i
		pitch += speed * 4;
		//glm::mat3 pitchMatrix = MatrixHelper::pitchMatrix(-speed * 5);
		////cameraPos = pitchMatrix * cameraPos;
		//cameraFront = pitchMatrix * cameraFront;
		//cameraUp = pitchMatrix * cameraUp;
		////cameraPos = glm::rotate(cameraPos, 0.01f, glm::vec3(0.0f,1.0f,0.0f))
	}

	if (keybuffer[107]) { //k
		pitch -= speed * 4;
		//glm::mat3 pitchMatrix = MatrixHelper::pitchMatrix(speed * 5);
		////cameraPos = pitchMatrix * cameraPos;
		//cameraFront = pitchMatrix * cameraFront;
		//cameraUp = pitchMatrix * cameraUp;
	}

	//clamp the pitch
	pitch = glm::clamp(pitch, -89.0f, 89.0f);

	//reset the yaw
	if (yaw >= 360 || yaw <= -360) {
		yaw = 0;
	}

	//rotate the camera using the the yaw and pitch
	glm::vec3 direction{};
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}