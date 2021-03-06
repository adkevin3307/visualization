#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "Method.h"
#include "Volume.h"

using namespace std;

class Slicing : public Method {
    using super = Method;

private:
    int _index;
    vector<vector<GLfloat>> _vertex;
    vector<float> _texture_2d, _texture_3d;
    vector<vector<float>> _template;
    glm::ivec3 _texture_2d_shape, _texture_3d_shape;

    void init();
    void generate_texture_2d();
    void generate_texture_3d();
    void push(glm::vec3 data, int index);
    void calculate();

public:
    Slicing();
    Slicing(Volume &volume);
    Slicing(string inf_file, string raw_file);
    ~Slicing();

    void run(int index);
    vector<float>& texture_2d();
    vector<float>& texture_3d();
    glm::ivec3 texture_2d_shape();
    glm::ivec3 texture_3d_shape();

    glm::vec3 shape() override;
    vector<GLfloat>& vertex() override;
    vector<int> attribute() override;
    GLenum render_mode() override;
};