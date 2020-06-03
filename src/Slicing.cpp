#include "Slicing.h"

#include "constant.h"

#include <iostream>
#include <fstream>

using namespace std;

Slicing::Slicing()
{

}

Slicing::Slicing(Volume &volume) : super::Method(volume)
{
    this->init();
}

Slicing::Slicing(string inf_file, string raw_file) : super::Method(inf_file, raw_file)
{
    super::volume.show();

    this->init();
}

Slicing::~Slicing()
{

}

void Slicing::init()
{
    this->_index = -1;
    this->_template = {
        {
            -1, 0, 0,
            -1, 1, 0,
            -1, 1, 1,
            -1, 0, 0,
            -1, 1, 1,
            -1, 0, 1
        },
        {
            0, -1, 0,
            1, -1, 0,
            1, -1, 1,
            0, -1, 0,
            1, -1, 1,
            0, -1, 1
        },
        {
            0, 0, -1,
            1, 0, -1,
            1, 1, -1,
            0, 0, -1,
            1, 1, -1,
            0, 1, -1
        }
    };

    this->generate_texture_1d();
    this->generate_texture_2d();
    this->generate_texture_3d();

    this->_vertex.resize(6);
    this->calculate();
}

void Slicing::generate_texture_1d()
{
    glm::vec2 limit_value = super::volume.limit_value();

    this->_texture_1d.resize((limit_value.y - limit_value.x + 1) * 4, 0.0);

    for (auto i = (74 - limit_value.x) * 4; i <= (86 - limit_value.x) * 4; i += 4) {
        this->_texture_1d[i + 0] = 0.0;
        this->_texture_1d[i + 1] = 1.0;
        this->_texture_1d[i + 2] = 0.0;
        this->_texture_1d[i + 3] = 0.1097;
    }

    for (auto i = (193 - limit_value.x) * 4; i <= (199 - limit_value.x) * 4; i += 4) {
        this->_texture_1d[i + 0] = 1.0;
        this->_texture_1d[i + 1] = 0.0;
        this->_texture_1d[i + 2] = 0.0;
        this->_texture_1d[i + 3] = 0.1387;
    }

    this->_texture_1d_shape = glm::ivec3((limit_value.y - limit_value.x + 1), 0, 0);
}

void Slicing::generate_texture_2d()
{
    int x, y;
    string temp;

    fstream file;
    file.open("transfer_function.txt", ios::in);

    file >> temp;
    file >> x >> y;
    this->_texture_2d.resize(x * y * 4);

    for (auto i = 0; i < x; i++) {
        for (auto j = 0; j < y; j += 4) {
            int index = (i * y + j) * 4;
            for (auto delta = 0; delta < 4; delta++) {
                file >> this->_texture_2d[index + delta];
            }
        }
    }

    file.close();

    this->_texture_2d_shape = glm::ivec3(x, y, 0);
}

void Slicing::generate_texture_3d()
{
    glm::ivec3 shape = super::volume.shape();
    glm::vec2 limit_value = super::volume.limit_value();

    this->_texture_3d.resize(shape.x * shape.y * shape.z * 4);

    for (auto i = 0; i < shape.x; i++) {
        for (auto j = 0; j < shape.y; j++) {
            for (auto k = 0; k < shape.z; k++) {
                int index = (k * shape.x * shape.y + j * shape.x + i) * 4;

                glm::vec3 normal = super::volume(i, j, k).second;
                float value = (super::volume(i, j, k).first - limit_value.x) / limit_value.y;

                glm::vec4 voxel = glm::vec4(normal, value);
                for (auto delta = 0; delta < 4; delta++) {
                    this->_texture_3d[index + delta] = voxel[delta];
                }
            }
        }
    }

    this->_texture_3d_shape = glm::ivec3(shape.x, shape.y, shape.z);
}

void Slicing::push(glm::vec3 data, int index)
{
    for (auto i = 0; i < 3; i++) {
        this->_vertex[index].push_back(data[i]);
    }
}

void Slicing::calculate()
{
    glm::ivec3 shape = super::volume.shape();
    glm::vec3 plane_position = glm::vec3(super::volume.shape()) * super::volume.voxel_size();

    double range = 0.5;

    for (size_t i = 0; i < this->_vertex.size(); i++) {
        int index = i / 2;
        double start = (i % 2 ? 0.0 : shape[index]);
        double delta = (i % 2 ? range : -range);

        for (auto j = 0; j < (shape[index] / range) + 1; j++, start += delta) {
            for (size_t k = 0; k < this->_template[index].size(); k += 3) {
                glm::vec3 basic = glm::vec3(
                    this->_template[index][k + 0],
                    this->_template[index][k + 1],
                    this->_template[index][k + 2]
                );

                glm::vec3 plane = basic * plane_position;
                glm::vec3 texture = basic;
                plane[index] = start;
                texture[index] = start / shape[index];

                this->push(plane, i);
                this->push(texture, i);
            }
        }
    }
}

int compare(glm::vec3 view_position)
{
    glm::vec3 temp = glm::vec3(abs(view_position[0]), abs(view_position[1]), abs(view_position[2]));

    if (temp[0] >= temp[1] && temp[0] >= temp[2]) return 0;
    if (temp[1] > temp[0] && temp[1] >= temp[2]) return 1;
    if (temp[2] > temp[0] && temp[2] > temp[1]) return 2;

    return -1;
}

bool Slicing::run(glm::vec3 view_position)
{
    int max_index = compare(view_position);
    int temp = max_index * 2 + (view_position[max_index] >= 0);

    if (this->_index == -1 || this->_index != temp) {
        this->_index = temp;
        return true;
    }

    return false;
}

vector<float>& Slicing::texture_1d()
{
    return this->_texture_1d;
}

vector<float>& Slicing::texture_2d()
{
    return this->_texture_2d;
}

vector<float>& Slicing::texture_3d()
{
    return this->_texture_3d;
}

glm::ivec3 Slicing::texture_1d_shape()
{
    return this->_texture_1d_shape;
}

glm::ivec3 Slicing::texture_2d_shape()
{
    return this->_texture_2d_shape;
}

glm::ivec3 Slicing::texture_3d_shape()
{
    return this->_texture_3d_shape;
}

glm::vec3 Slicing::shape()
{
    return glm::vec3(super::volume.shape()) * super::volume.voxel_size();
}

vector<GLfloat>& Slicing::vertex()
{
    return this->_vertex[this->_index];
}

vector<int> Slicing::attribute()
{
    return vector<int>{3, 3};
}

GLenum Slicing::render_mode()
{
    return GL_TRIANGLES;
}