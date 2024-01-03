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

//struct for vertex
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords; 
    Vertex();
    Vertex(glm::vec3);
};

//struct for material
struct Material
{
    string name;
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular;
    float power;
};

/// <summary>
/// mesh clash that is the parent class to all the rendered objects whether it be primities or modeled objects
/// The render methods for primitives and modeled objects are slightly different (although very similair) so Model overloads with its own render function
/// both primitives and models use GL draw triangles to render
/// Each Mesh has it's own material and has a reference to a shader, so implementation of multiple shaders is very easy
/// </summary>
class Mesh {
public:
    float pitch, yaw, roll; //transformation data, to be implemented!!
    glm::vec3 scale;
    glm::vec3 position;
    void Render(glm::mat4&);
    Mesh();
    Mesh(const char* imagepath, Shader* shader, Material material);
    void Rotate(float, glm::vec3);
    void Scale(glm::vec3);
    void Translate(glm::vec3);
protected:
    Material material;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    glm::mat4 model_matrix;
    Shader* shader; //shader pointer
    void setupPrimitive();
    void calculateNormals();
    unsigned int vao, vbo, ebo; //render data
    GLuint texture_id; //texture id
    
};

//box primitive
class Box : public Mesh {
public:
    Box();
    Box(glm::vec3, float, float, float, const char* imagepath, Shader* shader, Material material);
};

//pyramid primitive
class Pyramid : public Mesh {
public:
    Pyramid();
    Pyramid(glm::vec3, float, float, float, const char* imagepath, Shader* shader, Material material);

};

//trapezoid primitive 
class StairTrapezoid : public Mesh {
public:
    StairTrapezoid();
    StairTrapezoid(glm::vec3, float, float, float, float, const char* imagepath, Shader* shader, Material material);
};

//model class
class Model : public Mesh {
public:
    Model();
    Model(const char* modelpath, const char* texturepath, Shader* shader, Material material);
    void Render(glm::mat4);
private:
    void setupModel();
    
};

