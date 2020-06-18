#include "MeshManagement.h"

#include "constant.h"

#include "Transformation.h"

using namespace std;

vector<Mesh> MeshManagement::mesh;
vector<bool> MeshManagement::enable;

void MeshManagement::add(Mesh &rhs)
{
    MeshManagement::enable.push_back(true);
    MeshManagement::mesh.push_back(rhs);
}

void MeshManagement::update(glm::vec3 view_position)
{
    int max_index;
    glm::vec3 temp = glm::vec3(abs(view_position[0]), abs(view_position[1]), abs(view_position[2]));

    if (temp[0] >= temp[1] && temp[0] >= temp[2]) max_index = 0;
    if (temp[1] > temp[0] && temp[1] >= temp[2]) max_index = 1;
    if (temp[2] > temp[0] && temp[2] > temp[1]) max_index = 2;

    int index = max_index * 2 + (view_position[max_index] >= 0);

    int count = 0;
    for (size_t i = 0; i < MeshManagement::enable.size(); i++) {
        if (MeshManagement::mesh[i].method() == METHOD::SLICING) {
            if (count == index) MeshManagement::enable[i] = true;
            else MeshManagement::enable[i] = false;

            count += 1;
        }

        count %= 6;
    }
}

void MeshManagement::draw(map<METHOD, Shader> &shader_map, glm::mat4 view_matrix, double rate)
{
    for (size_t i = 0; i < MeshManagement::mesh.size(); i++) {
        if (MeshManagement::enable[i] == false) continue;

        Transformation transformation(shader_map[MeshManagement::mesh[i].method()]);
        transformation.set_projection(WIDTH, HEIGHT, rate, 0.0, 500.0);
        transformation.set_view(view_matrix);

        MeshManagement::mesh[i].transform(transformation);
        transformation.set();

        if (MeshManagement::mesh[i].method() == METHOD::ISOSURFACE) {
            shader_map[MeshManagement::mesh[i].method()].set_uniform("object_color", glm::vec4(0.41, 0.37, 0.89, 1.0));
        }
        MeshManagement::mesh[i].draw(GL_FILL);
    }
}

void MeshManagement::clear()
{
    MeshManagement::mesh.clear();
}