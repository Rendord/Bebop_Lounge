#include "Mesh.h"
/// <summary>
/// summary in the header file
/// </summary>
Vertex::Vertex()
{
}

Vertex::Vertex(glm::vec3 position)
{
	//set position
	this->position = position;
	//set normal to zero
	normal = glm::vec3(0.0,0.0,0.0);
	//set default texture coordinates for Mesh to 0:0
	texCoords = glm::vec2(0.0f, 0.0f);
}

Mesh::Mesh(){
}

Mesh::Mesh(const char* imagepath, Shader* shader, Material material)
{
	std::cout << imagepath << std::endl;
	//define model matrix
	model_matrix = glm::mat4();
	//load texture
	texture_id = loadBMP(imagepath);
	//add reference to shader
	this->shader = shader;
	this->material = material;

	//initialize transformation variables
	position = glm::vec3(0, 0, 0);
	pitch = 0;
	yaw = 0;
	roll = 0;
	scale = glm::vec3(1,1,1);
	
}

void Mesh::Rotate(float degrees, glm::vec3 axes)
{
	if (axes == glm::vec3(1, 0, 0)) {
		pitch += degrees;
	}
	if (axes == glm::vec3(0, 1, 0))
		yaw += degrees;
	if (axes == glm::vec3(0, 0, 1))
		roll += degrees;
	
}

void Mesh::Scale(glm::vec3 dimensions)
{
	//apply scale vector to model matrix
	scale *= dimensions; 
}

void Mesh::Translate(glm::vec3 translation)
{
	//apply Translation to position and then apply translation to model matrix
	position = position + translation;
}

void Mesh::Render(glm::mat4 &view)
{
	//activate shader
	shader->Use();
	//
	//calculate model_matrix
	glm::mat4 model_matrix = glm::mat4();

	//translation
	model_matrix = glm::translate(model_matrix, position);
	//rotation pitch
	model_matrix = glm::rotate(model_matrix, glm::radians(pitch), glm::vec3(1, 0, 0));
	//rotation yaw
	model_matrix = glm::rotate(model_matrix, glm::radians(yaw), glm::vec3(0, 1, 0));
	//rotation roll
	model_matrix = glm::rotate(model_matrix, glm::radians(roll), glm::vec3(0, 0, 1));
	//scale
	model_matrix = glm::scale(model_matrix, scale);

	//calculate the model view (change later to seperate model and view for clarity, only doing the MVP calculation in the shader)
	glm::mat4 mv = view * model_matrix;

	//set uniform 
	shader->setMat4("mv", mv);
	//bind texture
	glBindTexture(GL_TEXTURE_2D, texture_id);
	// draw mesh
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	//unbind
	glBindVertexArray(0);
}

void Mesh::setupPrimitive()
{
	//generate vertex array object (vao) and buffer objects for vertices (vbo) and indices (ebo)
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	//bind the vao
	glBindVertexArray(vao);
	//bind the buffer for the vertices
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//fill the buffer for the vertices
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	//bind the buffer for the indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//fill the buffer for the indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	//unbind vao
	glBindVertexArray(0);
}

void Mesh::calculateNormals() {
	//for each triangle in indices
	for (int i = 0; i < indices.size(); i += 3)
	{
		//grab vertices based on indices
		Vertex A = vertices[indices[i]];
		Vertex B = vertices[indices[i + 1]];
		Vertex C = vertices[indices[i + 2]];

		//calculate a normal (either inward or outward facing) based on the triangle
		glm::vec3 triNorm = glm::cross(B.position - A.position, C.position - A.position);

		
		//check if normal is facing inward and reverse it if it is
		if (glm::dot(triNorm, A.position - this->position) < 0) {
			triNorm = triNorm * -1.0f;
			std::cout << "swapping normal " << "vertices: " << indices[i] << " " << indices[i+1] << " " << indices[i+2] << std::endl;
		}

		//add normal of triangle to the normal of each vertex of the triangle
		vertices[indices[i]].normal += triNorm;
		vertices[indices[i + 1]].normal += triNorm;
		vertices[indices[i + 2]].normal += triNorm;


	};

	//normalize each normal
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].normal = glm::normalize(vertices[i].normal);
	}
}

Pyramid::Pyramid()
{
}

//sets up a Pyramid primitive
Pyramid::Pyramid(glm::vec3 pos, float length, float height, float width, const char* imagepath, Shader* shader, Material material) : Mesh(imagepath, shader, material)
{
	//calculates vertex positions based on height width and length, the position is equal to the center of the primitive
	std::cout << "constructing primitive mesh: " << typeid(this).name() << std::endl;
	//this->position = pos;
	vertices[0].position = glm::vec3(pos.x - length / 2.0, pos.y, pos.z - width / 2.0);
	vertices[1] = Vertex(glm::vec3(pos.x + length / 2.0, pos.y, pos.z - width / 2.0));
	vertices[2] = Vertex(glm::vec3(pos.x - length / 2.0, pos.y, pos.z + width / 2.0));
	vertices[3] = Vertex(glm::vec3(pos.x + length / 2.0, pos.y, pos.z + width / 2.0));
	vertices[4] = Vertex(glm::vec3(pos.x, pos.y + height, pos.z));

	
	//triangle indices
	indices = {
		0, 1, 2,
		1, 2, 3,
		0, 1, 4,
		0, 2, 4,
		1, 3, 4,
		2, 3, 4
	};

	//calculate normals and setup the vao
	calculateNormals();
	setupPrimitive();
}

Box::Box()
{
}

Box::Box(glm::vec3 pos, float length, float height, float width, const char* imagepath, Shader* shader, Material material) : Mesh(imagepath, shader, material)
{
	std::cout << "constructing primitive mesh: " << typeid(this).name() << std::endl;
	//this->position = pos;

	//calculates vertices based on height width and length, the position is equal to the center of the primitive
	vertices = {
	Vertex(glm::vec3(pos.x - length / 2.0, pos.y - height / 2.0, pos.z - width / 2.0)),
	Vertex(glm::vec3(pos.x + length / 2.0, pos.y - height / 2.0, pos.z - width / 2.0)),
	Vertex(glm::vec3(pos.x - length / 2.0, pos.y - height / 2.0, pos.z + width / 2.0)),
	Vertex(glm::vec3(pos.x + length / 2.0, pos.y - height / 2.0, pos.z + width / 2.0)),
	Vertex(glm::vec3(pos.x - length / 2.0, pos.y + height / 2.0, pos.z - width / 2.0)),
	Vertex(glm::vec3(pos.x + length / 2.0, pos.y + height / 2.0, pos.z - width / 2.0)),
	Vertex(glm::vec3(pos.x - length / 2.0, pos.y + height / 2.0, pos.z + width / 2.0)),
	Vertex(glm::vec3(pos.x + length / 2.0, pos.y + height / 2.0, pos.z + width / 2.0))
	};

	//triangle indices
	indices = {
	    0, 1, 2,
		3, 2, 1,
		4, 1, 0,
		1, 4, 5,
		0, 2, 4,
		6, 4, 2,
		5, 3, 1,
		3, 5, 7,
		2, 3, 6,
		7, 6, 3,
		6, 5, 4,
		5, 6, 7
	};

	//calculate normals and setup the vao
	calculateNormals();
	setupPrimitive();
}


StairTrapezoid::StairTrapezoid()
{
}

StairTrapezoid::StairTrapezoid(glm::vec3 pos, float length, float height, float width, float edgelength, const char* texturepath, Shader* shader, Material material) : Mesh(texturepath, shader, material)
{
	std::cout << "constructing primitive mesh: " << typeid(this).name() << std::endl;
	//this->position = pos;

	//calculates vertices based on the height width and length and edge length (edge length is the length of the far side of the triangle that form the sides of the trapezoid
	//				   trapezoid  |
	//							  v			
	//						_________________
	//						/				\
	//	edge length	-->	   /				 \		<----- edge length
	//					  /					  \
	//					 /					   \
	//					/					    \
	//				   ---------------------------
	//
	//
	vertices = {
	Vertex(glm::vec3(pos.x - length / 2.0, pos.y - height / 2.0, pos.z - width / 2.0)),
	Vertex(glm::vec3(pos.x + length / 2.0, pos.y + height / 2.0, pos.z - width / 2.0)),
	Vertex(glm::vec3(pos.x + length / 2.0, (pos.y + height / 2.0) - edgelength, pos.z - width / 2.0)),
	Vertex(glm::vec3((pos.x - length / 2.0) + edgelength, pos.y - height / 2.0, pos.z - width / 2.0)),
	Vertex(glm::vec3(pos.x - length / 2.0, pos.y - height / 2.0, pos.z + width / 2.0)),
	Vertex(glm::vec3(pos.x + length / 2.0, pos.y + height / 2.0, pos.z + width / 2.0)),
	Vertex(glm::vec3(pos.x + length / 2.0, (pos.y + height / 2.0) - edgelength, pos.z + width / 2.0)),
	Vertex(glm::vec3((pos.x - length / 2.0) + edgelength, pos.y - height / 2.0, pos.z + width / 2.0)),
	};

	//triangle indices
	indices = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 7,
		0, 7, 4,
		2, 1, 5,
		2, 5, 6,
		3, 2, 6,
		3, 6, 7,
		4, 6, 5,
		4, 7, 6,
		0, 1, 5,
		0, 5, 4
	};

	//calculate normals and setup the vao
	calculateNormals();
	setupPrimitive();


}

Model::Model()
{
}


Model::Model(const char* modelpath, const char* texturepath, Shader* shader, Material material) : Mesh(texturepath, shader, material)
{

	std::cout << "constructing mesh: " << typeid(this).name() << std::endl;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;

	//loadOBJ function
	loadOBJ(modelpath, vertices, uvs, normals);

	//check if the vectors that hold uvs, vertices and normals are the same size after loading
	if (!(normals.size() == vertices.size() && vertices.size() == uvs.size() && normals.size() == uvs.size())) {
		std::cout << "oh oh, we cant use this model" << std::endl;
		return;
	}

	//make Vertex structs out of the data and fill the vertices vector of mesh
	for (int i = 0; i < vertices.size(); i++) {
		Vertex v;
		v.position = vertices[i];
		v.normal = normals[i];
		v.texCoords = uvs[i];
		this->vertices.push_back(v);
	}

	setupModel();

}


void Model::setupModel()
{
	std::cout << "setting up model" << std::endl;
	//generate vertex array object (vao)
	glGenVertexArrays(1, &vao);
	//generate vertex buffer object (vbo)
	glGenBuffers(1, &vbo);

	//bind vao and vbo
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//fill buffer with vertices
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);


	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex uv
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void Model::Render(glm::mat4 view)
{
	//activate shader
	shader->Use();
	//calculate the model view (change later to seperate model and view for clarity, only doing the MVP calculation in the shader)

	//calculate model_matrix
	glm::mat4 model_matrix = glm::mat4();
	

	//translation
	model_matrix = glm::translate(model_matrix, position);

	//rotation pitch
	model_matrix = glm::rotate(model_matrix, glm::radians(pitch), glm::vec3(1, 0, 0));
	//rotation yaw
	model_matrix = glm::rotate(model_matrix, glm::radians(yaw), glm::vec3(0, 1, 0));
	//rotation roll
	model_matrix = glm::rotate(model_matrix, glm::radians(roll), glm::vec3(0, 0, 1));
	
	//scale
	model_matrix = glm::scale(model_matrix, scale);

	glm::mat4 mv = view * model_matrix;
	//set uniform 
	shader->setMat4("mv", mv);
	shader->setVec3("mat_specular", material.specular);
	shader->setVec3("mat_ambient", material.ambient_color);
	shader->setVec3("mat_diffuse", material.diffuse_color);
	shader->setFloat("mat_power", material.power);
	//bind texture
	glBindTexture(GL_TEXTURE_2D, texture_id);
	// draw mesh
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	//unbind 
	glBindVertexArray(0);
}


