#pragma once

#include "constant.h"

#include <glm/glm.hpp>
#include <vector>
#include <map>

#include "Mesh.h"
#include "Shader.h"

using namespace std;

class MeshManagement {
private:
    static vector<Mesh> mesh;
    static vector<bool> enable;

public:
    static void add(Mesh &rhs);
    static void update(glm::vec3 view_position, METHOD method);
    static void draw(map<METHOD, Shader> &shader_map, glm::mat4 view_matrix, double rate);
    static void clear();
};