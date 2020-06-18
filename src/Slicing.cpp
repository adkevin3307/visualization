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

    this->generate_texture_2d();
    this->generate_texture_3d();

    this->_vertex.resize(6);
    this->calculate();
}

void Slicing::generate_texture_2d()
{
    int x, y;
    bool equalization;
    string volume_name;

    fstream file;
    file.open("transfer_function.txt", ios::in);

    file >> volume_name;
    file >> equalization;
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

    if (equalization) super::volume.equalization();
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
                plane[index] = start * super::volume.voxel_size()[index];
                texture[index] = start / shape[index];

                this->push(plane, i);
                this->push(texture, i);
            }
        }
    }
}

void Slicing::run(int index)
{
    this->_index = index;
}

vector<float>& Slicing::texture_2d()
{
    return this->_texture_2d;
}

vector<float>& Slicing::texture_3d()
{
    return this->_texture_3d;
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