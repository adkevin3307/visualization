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
    vector<vector<glm::vec2>> data;
    vector<GLfloat> _vertex;

    void load_data(string filename);
    bool check(glm::vec2 position);
    glm::vec2 vector_interpolation(glm::vec2 position);
    glm::vec2 rk2(glm::vec2 position, float h);
    vector<GLfloat> calculate(glm::vec2 position, float delta, vector<vector<bool>> &table);

public:
    StreamLine(string filename);
    ~StreamLine();

    void run();
    glm::vec3 shape() override;
    vector<GLfloat>& vertex() override;
    vector<int> attribute() override;
    GLenum render_mode() override;
};