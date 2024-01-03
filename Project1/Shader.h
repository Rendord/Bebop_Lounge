#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "glsl.h"
#include <iostream>
/// <summary>
/// Shader class that handles the setting of the uniforms and holds the program id.
/// Its constructor builds a Shader program from the given vertex shader and fragment shader files
/// </summary>
class Shader {
private:
	GLuint program_id;
public:
	Shader();
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void Use();
	void setBool(const char* name, bool value);
	void setInt(const char* name, int value);
	void setFloat(const char* name, float value);
	void setVec2(const char* name, const glm::vec2 &value);
	void setVec3(const char* name, const glm::vec3 &value);
	void setVec4(const char* name, const glm::vec4 &value);
	void setMat2(const char* name, const glm::mat2 &mat);
	void setMat3(const char* name, const glm::mat3 &mat);
	void setMat4(const char* name, const glm::mat4 &mat);
	GLuint getProgramId();



};