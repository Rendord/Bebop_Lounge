#include "PrimitiveModel.h"

PrimitiveModel::PrimitiveModel()
{
}

void PrimitiveModel::AddMesh(Mesh mesh)
{
	meshes.push_back(mesh);
}

void PrimitiveModel::Rotate(float degrees, glm::vec3 axes)
{
	//rotate all the meshes
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Rotate(degrees, axes);
	}
}

void PrimitiveModel::Translate(glm::vec3 translation)
{
	//translate all the meshes
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Translate(translation);
	}
}

void PrimitiveModel::Scale(glm::vec3 dimensions)
{
	//scales all the meshes
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Scale(dimensions);
	}
}

void PrimitiveModel::Render(glm::mat4 &view)
{
	//renders all the meshes
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Render(view);
	}
}


