#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Transformation.h"
#include "BufferManagement.h"

using namespace std;

class Mesh {
private:
    int stride;
    Buffer buffer;
    glm::vec3 shape;
    GLenum render_mode;
    vector<GLfloat> vertex;
    vector<int> attribute;

public:
    Mesh();
    Mesh(vector<GLfloat> &vertex, vector<int> attribute, glm::vec3 shape = glm::vec3(0.0), GLenum render_mode = GL_TRIANGLES);
    ~Mesh();

    void init();
    void transform(Transformation &transformation);
    void draw(GLenum rasterize_mode);
};