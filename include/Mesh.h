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
    GLenum render_mode;
    vector<int> attribute;
    vector<GLfloat> vertex;

public:
    Mesh();
    Mesh(Method *method);
    ~Mesh();

    void init();
    void transform(Transformation &transformation);
    void color(Shader &shader, glm::vec4 color = glm::vec4(0.41, 0.37, 0.89, 1.0));
    void draw(GLenum rasterize_mode);
};