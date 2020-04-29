#include "Slicing.h"

#define _USE_MATH_DEFINES

#include "constant.h"

#include <cmath>

using namespace std;

Slicing::Slicing(string inf_file, string raw_file) : super::Method(inf_file, raw_file)
{
    super::volume.show();

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
    glm::vec2 limit_value = super::volume.limit_value();

    this->_texture_1d.resize((limit_value.y - limit_value.x + 1) * 4, 0.0);

    // for (size_t i = 0; i < this->_texture_1d.size(); i += 4) {
    //     this->_texture_1d[i + 0] = gaussian((20.0 - limit_value.x) * 4.0, 50.0, i) * 50.0;
    //     this->_texture_1d[i + 1] = gaussian((80.0 - limit_value.x) * 4.0, 50.0, i) * 50.0;
    //     this->_texture_1d[i + 2] = gaussian((150.0 - limit_value.x) * 4.0, 50.0, i) * 50.0;
    //     this->_texture_1d[i + 3] = gaussian((80.0 - limit_value.x) * 4.0, 5.0, i) * 5.0;
    // }

    int render_value = (int)super::volume.average();

    this->_texture_1d[(render_value - limit_value.x) * 4 + 0] = 0.41;
    this->_texture_1d[(render_value - limit_value.x) * 4 + 1] = 0.37;
    this->_texture_1d[(render_value - limit_value.x) * 4 + 2] = 0.89;
    this->_texture_1d[(render_value - limit_value.x) * 4 + 3] = 0.5;
}

void Slicing::generate_texture_3d()
{
    glm::ivec3 shape = super::volume.shape();
    glm::vec2 limit_value = super::volume.limit_value();

    this->_texture_3d.resize(shape.x * shape.y * shape.z * 4);

    for (auto i = 0; i < shape.x; i++) {
        for (auto j = 0; j < shape.y; j++) {
            for (auto k = 0; k < shape.z; k++) {
                int index = (i * shape.y * shape.z + j * shape.z + k) * 4;

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

void Slicing::run()
{
    glm::ivec3 shape = super::volume.shape();
    glm::vec3 plane_position = glm::vec3(shape) * super::volume.voxel_size();

    for (double index = 0; index < shape.z; index += 0.1) {
        this->push(glm::vec3(0.0, 0.0, index));
        this->push(glm::vec3(0.0, 0.0, index / shape.z));

        this->push(glm::vec3(plane_position.x, 0.0, index));
        this->push(glm::vec3(1.0, 0.0, index / shape.z));

        this->push(glm::vec3(plane_position.x, plane_position.y, index));
        this->push(glm::vec3(1.0, 1.0, index / shape.z));

        this->push(glm::vec3(0.0, 0.0, index));
        this->push(glm::vec3(0.0, 0.0, index / shape.z));

        this->push(glm::vec3(plane_position.x, plane_position.y, index));
        this->push(glm::vec3(1.0, 1.0, index / shape.z));

        this->push(glm::vec3(0.0, plane_position.y, index));
        this->push(glm::vec3(0.0, 1.0, index / shape.z));
    }
}

void Slicing::run(double index)
{
    this->_vertex.clear();
    this->_vertex.shrink_to_fit();

    glm::ivec3 shape = super::volume.shape();
    glm::vec3 plane_position = glm::vec3(shape) * super::volume.voxel_size();

    this->push(glm::vec3(0.0, 0.0, index));
    this->push(glm::vec3(0.0, 0.0, index / shape.z));

    this->push(glm::vec3(plane_position.x, 0.0, index));
    this->push(glm::vec3(1.0, 0.0, index / shape.z));

    this->push(glm::vec3(plane_position.x, plane_position.y, index));
    this->push(glm::vec3(1.0, 1.0, index / shape.z));

    this->push(glm::vec3(0.0, 0.0, index));
    this->push(glm::vec3(0.0, 0.0, index / shape.z));

    this->push(glm::vec3(plane_position.x, plane_position.y, index));
    this->push(glm::vec3(1.0, 1.0, index / shape.z));

    this->push(glm::vec3(0.0, plane_position.y, index));
    this->push(glm::vec3(0.0, 1.0, index / shape.z));
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