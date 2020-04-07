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
    vector<GLfloat> vertex;

public:
    Mesh();
    Mesh(vector<GLfloat> &vertex, glm::vec3 shape = glm::vec3(0.0, 0.0, 0.0), int stride = 6);
    ~Mesh();

    void init();
    void transform(Transformation &transformation);
    void draw(GLenum mode);
};