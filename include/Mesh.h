#pragma once

#include "constant.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Transformation.h"
#include "BufferManagement.h"
#include "Method.h"

using namespace std;

class Mesh {
private:
    int stride;
    Buffer buffer;
    glm::vec3 shape;
    GLenum render_mode;
    vector<int> attribute;
    vector<GLfloat> vertex;
    METHOD _method;

public:
    Mesh();
    Mesh(Method *render_method, METHOD method);
    ~Mesh();

    METHOD method();

    void init();
    void transform(Transformation &transformation);
    void draw(GLenum rasterize_mode);
};