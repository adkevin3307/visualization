#include "MeshManagement.h"

#include "Transformation.h"

using namespace std;

vector<Mesh> MeshManagement::mesh;
vector<bool> MeshManagement::enable;

void MeshManagement::add(Mesh &rhs)
{
    MeshManagement::enable.push_back(true);
    MeshManagement::mesh.push_back(rhs);
}

void MeshManagement::update(glm::vec3 view_position, METHOD method)
{
    int max_arg_index;
    glm::vec3 temp = glm::abs(view_position);

    if (temp[0] >= temp[1] && temp[0] >= temp[2]) max_arg_index = 0;
    if (temp[1] > temp[0] && temp[1] >= temp[2]) max_arg_index = 1;
    if (temp[2] > temp[0] && temp[2] > temp[1]) max_arg_index = 2;

    int index = max_arg_index * 2 + (view_position[max_arg_index] >= 0);

    for (size_t i = 0; i < MeshManagement::enable.size(); i++) {
        MeshManagement::enable[i] = (MeshManagement::mesh[i].method() == method);

        if (method == METHOD::SLICING && MeshManagement::mesh[i].method() == METHOD::SLICING) {
            for (auto count = 0; count < 6; count++) {
                MeshManagement::enable[i + count] = (count == index);
            }
            i += 5;
        }
    }
}

void MeshManagement::draw(map<METHOD, Shader> &shader_map, glm::mat4 view_matrix, double rate)
{
    for (size_t i = 0; i < MeshManagement::mesh.size(); i++) {
        if (MeshManagement::enable[i] == false) continue;

        METHOD method = MeshManagement::mesh[i].method();

        Transformation transformation(shader_map[method]);
        transformation.set_projection(WIDTH, HEIGHT, rate, -5000.0, 5000.0);
        transformation.set_view(view_matrix);

        MeshManagement::mesh[i].transform(transformation);
        transformation.set((method == METHOD::ISOSURFACE || method == METHOD::SLICING));

        if (method == METHOD::ISOSURFACE) {
            shader_map[method].set_uniform("object_color", glm::vec4(0.41, 0.37, 0.89, 1.0));
        }
        MeshManagement::mesh[i].draw(GL_FILL);
    }
}

void MeshManagement::clear()
{
    MeshManagement::mesh.clear();
    MeshManagement::enable.clear();
}