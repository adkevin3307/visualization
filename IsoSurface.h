#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iomanip>

#include "Volume.h"

using namespace std;

class IsoSurface {
private:
    float iso_value;
    Volume volume;
    vector<GLfloat> _vertex;

    pair<glm::vec3, glm::vec3> interpolation(glm::ivec3 p1, glm::ivec3 p2);
    void calculate();
    
public:
    IsoSurface(string inf_file, string raw_file);
    ~IsoSurface();

    void run();
    void run(float value);
    vector<GLfloat>& vertex();

    glm::vec3 volume_shape();
    vector<int> attribute_size();
};