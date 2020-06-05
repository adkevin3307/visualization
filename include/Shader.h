#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

using namespace std;

class Shader {
private:
    GLuint ID;

public:
    Shader();
    Shader(string vertex_path, string fragment_path, string geometry_path = "");
    ~Shader();

    void use();
    void set_uniform(const string &name, bool value) const;
    void set_uniform(const string &name, int value) const;
    void set_uniform(const string &name, float value) const;
    void set_uniform(const string &name, glm::vec3 value) const;
    void set_uniform(const string &name, glm::vec4 value) const;
    void set_uniform(const string &name, glm::mat4 value) const;
};