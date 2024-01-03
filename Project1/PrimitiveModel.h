#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GL/freeglut.h>

#include <string>
#include <vector>
#include <iostream>
#include "Shader.h"
#include "texture.h"
#include "objloader.h"
#include "Mesh.h"
/// <summary>
/// container class for a model that is build up out of multiple primitives
/// it has one function that pushes the mesh into the vector and all the other functions
/// just call their respective functions on all the elements of the vector
/// </summary>
class PrimitiveModel {
private:
	vector<Mesh> meshes;
public:
	PrimitiveModel();
	void AddMesh(Mesh);
	void Rotate(float, glm::vec3);
	void Translate(glm::vec3);
	void Scale(glm::vec3);
	void Render(glm::mat4&);

};