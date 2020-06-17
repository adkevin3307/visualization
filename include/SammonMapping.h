#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "Method.h"

using namespace std;

class SammonMapping : public Method {
    using super = Method;

private:
    int group;
    vector<int> label;
    vector<vector<float>> data, _distance;
    vector<GLfloat> _vertex;

    float norm_2(vector<float> p1, vector<float> p2);
    void normalize();
    void kmeans(int group);
    void calculate_distance();
    void load_custom(string filename);
    void load_scalar();
    glm::vec2 descent(float lambda, glm::ivec2 index, vector<glm::vec2> &mapping_point);

public:
    SammonMapping();
    SammonMapping(string filename);
    SammonMapping(string inf_file, string raw_file);
    ~SammonMapping();

    void run(float alpha);
    glm::vec3 shape() override;
    vector<GLfloat>& vertex() override;
    vector<int> attribute() override;
    GLenum render_mode() override;
};