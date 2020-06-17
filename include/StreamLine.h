#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "Method.h"

using namespace std;

class StreamLine : public Method {
    using super = Method;

private:
    int _max_scale, _base_scale;
    float min_vector_magnitude, max_vector_magnitude;
    vector<vector<glm::vec2>> data;
    vector<vector<vector<bool>>> tables;
    vector<GLfloat> _vertex;

    void load_data(string filename);
    bool inside(glm::vec2 position);
    glm::vec2 vector_interpolation(glm::vec2 position);
    void push(glm::vec2 position, vector<GLfloat> &result);
    glm::ivec2 data2table(glm::vec2 position, int scale);
    glm::vec2 rk2(glm::vec2 position, float h);
    vector<GLfloat> calculate(glm::vec2 position, float delta, int scale);

public:
    StreamLine(string filename);
    ~StreamLine();

    int base_scale();
    int max_scale();

    void run();
    glm::vec3 shape() override;
    vector<GLfloat>& vertex() override;
    vector<int> attribute() override;
    GLenum render_mode() override;
};