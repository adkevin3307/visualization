#include "Slicing.h"

#define _USE_MATH_DEFINES

#include "constant.h"

#include <cmath>

using namespace std;

Slicing::Slicing()
{

}

Slicing::Slicing(string inf_file, string raw_file) : super::Method(inf_file, raw_file)
{
    super::volume.show();

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
    this->generate_texture_3d();
}

Slicing::~Slicing()
{

}

double gaussian(double mu, double sigma, double value)
{
    double coefficient = sigma * sqrt(2 * M_PI);
    double temp = exp(-1 * (value - mu) * (value - mu) / (2 * sigma * sigma));

    return temp / coefficient;
}

void Slicing::generate_texture_1d()
{
    // double render_value = super::volume.average();
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

    // for (size_t i = 0; i < this->_texture_1d.size(); i += 4) {
    //     this->_texture_1d[i + 0] = gaussian((render_value - limit_value.x) * 4.0, 5.0, i) * 5.0;
    //     this->_texture_1d[i + 1] = gaussian((render_value - limit_value.x) * 4.0, 5.0, i) * 5.0;
    //     this->_texture_1d[i + 2] = gaussian((render_value - limit_value.x) * 4.0, 5.0, i) * 5.0;
    //     this->_texture_1d[i + 3] = gaussian((render_value - limit_value.x) * 4.0, 5.0, i) * 5.0;
    // }
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
}

void Slicing::push(glm::vec3 data)
{
    for (auto i = 0; i < 3; i++) {
        this->_vertex.push_back(data[i]);
    }
}

void Slicing::calculate(double index, int max_index, glm::ivec3 shape)
{
    glm::vec3 plane_position = super::volume_shape();

    for (size_t i = 0; i < this->_template[max_index].size(); i += 3) {
        glm::vec3 basic = glm::vec3(
            this->_template[max_index][i + 0],
            this->_template[max_index][i + 1],
            this->_template[max_index][i + 2]
        );

        glm::vec3 plane = basic * plane_position;
        glm::vec3 texture = basic;
        plane[max_index] = index;
        texture[max_index] = index / shape[max_index];

        this->push(plane);
        this->push(texture);
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

void Slicing::run(glm::vec3 view_position)
{
    this->_vertex.clear();
    this->_vertex.shrink_to_fit();

    int max_index = compare(view_position);
    glm::ivec3 shape = super::volume.shape();

    double range = 0.5;
    int amount = (shape[max_index] / range) + 1;
    double index = (view_position[max_index] >= 0 ? 0.0 : shape[max_index]);
    double delta = (view_position[max_index] >= 0 ? range : -range);

    for (auto i = 0; i < amount; i++, index += delta) {
        this->calculate(index, max_index, shape);
    }
}

vector<float>& Slicing::texture_1d()
{
    return this->_texture_1d;
}

vector<float>& Slicing::texture_3d()
{
    return this->_texture_3d;
}

glm::ivec3 Slicing::texture_1d_shape()
{
    return glm::ivec3(this->_texture_1d.size() / 4, 0, 0);
}

glm::ivec3 Slicing::texture_3d_shape()
{
    return super::volume.shape();
}

vector<GLfloat>& Slicing::vertex()
{
    return this->_vertex;
}

vector<int> Slicing::attribute()
{
    return vector<int>{3, 3};
}

GLenum Slicing::render_mode()
{
    return GL_TRIANGLES;
}