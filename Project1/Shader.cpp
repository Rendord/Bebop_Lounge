#include "Shader.h"
/// <summary>
/// summary in the header file
/// </summary>
Shader::Shader()
{
}

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
    std::cout << "reading shader files" << std::endl;
    char* vertexshader = glsl::readFile(vertexShaderPath);
    GLuint vsh_id = glsl::makeVertexShader(vertexshader);

    char* fragshader = glsl::readFile(fragmentShaderPath);
    GLuint fsh_id = glsl::makeFragmentShader(fragshader);

    program_id = glsl::makeShaderProgram(vsh_id, fsh_id);
    std::cout << program_id << std::endl;
}

void Shader::Use()
{
    glUseProgram(program_id);
}

void Shader::setBool(const char* name, bool value)
{
    glUniform1i(glGetUniformLocation(program_id, name), (int)value);
}

void Shader::setInt(const char* name, int value)
{
    glUniform1i(glGetUniformLocation(program_id, name), value);
}

void Shader::setFloat(const char* name, float value)
{
    glUniform1f(glGetUniformLocation(program_id, name), value);
}

void Shader::setVec2(const char* name, const glm::vec2& value)
{
    glUniform2fv(glGetUniformLocation(program_id, name), 1, &value[0]);
}

void Shader::setVec3(const char* name, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(program_id, name), 1, &value[0]);
}

void Shader::setVec4(const char* name, const glm::vec4& value)
{
    glUniform4fv(glGetUniformLocation(program_id, name), 1, &value[0]);
}

void Shader::setMat2(const char* name, const glm::mat2& mat)
{
    glUniformMatrix2fv(glGetUniformLocation(program_id, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const char* name, const glm::mat3& mat)
{
    glUniformMatrix3fv(glGetUniformLocation(program_id, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const char* name, const glm::mat4& mat)
{
    glUniformMatrix4fv(glGetUniformLocation(program_id, name), 1, GL_FALSE, &mat[0][0]);
}

GLuint Shader::getProgramId()
{
    return program_id;
}


