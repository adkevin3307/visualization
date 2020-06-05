#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

Shader::Shader()
{
    
}

Shader::Shader(string vertex_path, string fragment_path, string geometry_path)
{
    string vertex_code, fragment_code, geometry_code;
    fstream vertex_file, fragment_file, geometry_file;
    stringstream vertex_stream, fragment_stream, geometry_stream;

    vertex_file.open(vertex_path, ios::in);
    fragment_file.open(fragment_path, ios::in);
    if (geometry_path != "") geometry_file.open(geometry_path, ios::in);

    vertex_stream << vertex_file.rdbuf();
    fragment_stream << fragment_file.rdbuf();
    if (geometry_path != "") geometry_stream << geometry_file.rdbuf();

    vertex_code = vertex_stream.str();
    fragment_code = fragment_stream.str();
    if (geometry_path != "") geometry_code = geometry_stream.str();

    vertex_file.close();
    fragment_file.close();
    if (geometry_path != "") geometry_file.close();

    const char *vertex_code_temp = vertex_code.c_str(), *fragment_code_temp = fragment_code.c_str(), *geometry_code_temp;
    if (geometry_path != "") geometry_code_temp = geometry_code.c_str();

    GLuint vertex, fragment, geometry;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_code_temp, NULL);
    glCompileShader(vertex);

    GLint compile_vertex_success;
    GLchar compile_vertex_log[1024];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &compile_vertex_success);
    if (compile_vertex_success) cout << vertex_path << ": " << "SHADER::VERTEX::COMPILATION_SUCCESS" << '\n';
    else {
        glGetShaderInfoLog(vertex, 1024, NULL, compile_vertex_log);
        cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << compile_vertex_log << '\n';
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_code_temp, NULL);
    glCompileShader(fragment);

    GLint compile_fragment_success;
    GLchar compile_fragment_log[1024];
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &compile_fragment_success);
    if (compile_fragment_success) cout << fragment_path << ": " << "SHADER::FRAGMENT::COMPILATION_SUCCESS" << '\n';
    else {
        glGetShaderInfoLog(fragment, 1024, NULL, compile_fragment_log);
        cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: " << compile_fragment_log << '\n';
    }

    if (geometry_path != "") {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometry_code_temp, NULL);
        glCompileShader(geometry);

        GLint compile_geometry_success;
        GLchar compile_geometry_log[1024];
        glGetShaderiv(geometry, GL_COMPILE_STATUS, &compile_geometry_success);
        if (compile_geometry_success) cout << geometry_path << ": " << "SHADER::GEOMETRY::COMPILATION_SUCCESS" << '\n';
        else {
            glGetShaderInfoLog(geometry, 1024, NULL, compile_geometry_log);
            cerr << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED: " << compile_geometry_log << '\n';
        }
    }

    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertex);
    glAttachShader(this->ID, fragment);
    if (geometry_path != "") glAttachShader(this->ID, geometry);
    glLinkProgram(ID);

    GLint link_success;
    GLchar link_log[1024];
    glGetProgramiv(ID, GL_LINK_STATUS, &link_success);
    if (link_success) cout << "SHADER::PROGRAM::LINKING_SUCCESS" << '\n';
    else {
        glGetProgramInfoLog(ID, 1024, NULL, link_log);
        cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED: " << link_log << '\n';
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry_path != "") glDeleteShader(geometry);
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