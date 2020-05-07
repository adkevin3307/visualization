#pragma once

#include "constant.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <iomanip>

#include "Transformation.h"
#include "BufferManagement.h"
#include "Method.h"

using namespace std;

class Mesh {
private:
    int stride;
    bool use_texture;
    Buffer buffer;
    vector<pair<GLuint, GLenum>> texture;
    glm::vec3 shape;
    GLenum render_mode;
    vector<int> attribute;
    vector<GLfloat> vertex;
    METHOD _method;

public:
    Mesh();
    Mesh(Method &render_method, METHOD method);
    Mesh(vector<GLfloat> &vertex, vector<int> attribute, GLenum render_mode, glm::vec3 shape, METHOD method);
    ~Mesh();

    METHOD method();

    void init();
    void transform(Transformation &transformation);
    void draw(GLenum rasterize_mode);

    void enable_texture(int size = 1);
    void init_texture(GLenum target, int index);
    void set_texture(int index, vector<float> &texture_data, glm::ivec3 shape);
};