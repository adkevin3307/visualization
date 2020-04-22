#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>

using namespace std;

Shader::Shader()
{
    
}

Shader::Shader(string vertex_path, string fragment_path)
{
    string vertex_code, fragment_code;
    fstream vertex_file, fragment_file;
    stringstream vertex_stream, fragment_stream;

    vertex_file.open(vertex_path, ios::in);
    fragment_file.open(fragment_path, ios::in);

    vertex_stream << vertex_file.rdbuf();
    fragment_stream << fragment_file.rdbuf();

    vertex_code = vertex_stream.str();
    fragment_code = fragment_stream.str();

    vertex_file.close();
    fragment_file.close();

    const char *vertex_code_temp = vertex_code.c_str(), *fragment_code_temp = fragment_code.c_str();
    GLuint vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_code_temp, NULL);
    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_code_temp, NULL);
    glCompileShader(fragment);

    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertex);
    glAttachShader(this->ID, fragment);
    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
    
}

void Shader::use()
{
    glUseProgram(this->ID);
}

void Shader::set_uniform(const string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}

void Shader::set_uniform(const string &name, int value) const
{
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::set_uniform(const string &name, float value) const
{
    glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void Shader::set_uniform(const string &name, glm::vec3 value) const
{
    glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]);
}

void Shader::set_uniform(const string &name, glm::vec4 value) const
{
    glUniform4fv(glGetUniformLocation(this->ID, name.c_str()), 1, &value[0]);
}

void Shader::set_uniform(const string &name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}