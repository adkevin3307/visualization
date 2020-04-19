#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Shader.h"
#include "Transformation.h"
#include "BufferManagement.h"
#include "Method.h"

using namespace std;

class Mesh {
private:
    int stride;
    Buffer buffer;
    glm::vec3 shape;
    glm::vec4 _color;
    GLenum render_mode;
    vector<int> attribute;
    vector<GLfloat> vertex;

public:
    Mesh();
    Mesh(Method *method, glm::vec4 color);
    ~Mesh();

    void init();
    void transform(Transformation &transformation);
    void color(Shader &shader);
    void draw(GLenum rasterize_mode);
};