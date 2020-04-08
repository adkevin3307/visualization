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
    vector<int> attribute_size;

public:
    Mesh();
    Mesh(vector<GLfloat> &vertex, vector<int> attribute_size, glm::vec3 shape = glm::vec3(0.0, 0.0, 0.0));
    ~Mesh();

    void init();
    void transform(Transformation &transformation);
    void draw(GLenum mode);
};