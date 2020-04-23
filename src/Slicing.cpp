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
    this->_texture_1d.resize((abs(super::volume.limit_value().x - super::volume.limit_value().y) + 1) * 4);

    for (size_t i = 0; i < this->_texture_1d.size(); i += 4) {
        this->_texture_1d[i] = gaussian(80.0 * 4.0, 10.0, i) * 10.0;
        this->_texture_1d[i + 1] = gaussian(80.0 * 4.0, 10.0, i) * 10.0;
        this->_texture_1d[i + 2] = gaussian(80.0 * 4.0, 10.0, i) * 10.0;
        this->_texture_1d[i + 3] = gaussian(80.0 * 4.0, 200.0, i) * 200.0;
    }
}

void Slicing::generate_texture_3d()
{
    glm::ivec3 shape = super::volume.shape();

    this->_texture_3d.resize(shape.x * shape.y * shape.z * 4);

    for (auto i = 0; i < shape.x; i++) {
        for (auto j = 0; j < shape.y; j++) {
            for (auto k = 0; k < shape.z; k++) {
                int index = (i * shape.y * shape.z + j * shape.z + k) * 4;

                glm::vec3 normal = super::volume(i, j, k).second;
                if (glm::length(normal) > EPSILON) normal = glm::normalize(normal);

                float value = super::volume(i, j, k).first / super::volume.limit_value().y;

                glm::vec4 voxel = glm::vec4(normal, value);
                for (auto delta = 0; delta < 4; delta++) {
                    this->_texture_3d[index + delta] = voxel[delta];
                }
            }
        }
    }
}

void Slicing::push(double x, double y, double z)
{
    this->_vertex.push_back(x);
    this->_vertex.push_back(y);
    this->_vertex.push_back(z);
}

void Slicing::run()
{
    // axis
    glm::ivec3 shape = super::volume.shape();
    for (float i = 0; i < shape.z; i += 0.1) {
        double temp = i / shape.z;
        // triangle
        this->push(0.0, 0.0, (double)i);
        this->push(0.0, 0.0, temp);

        this->push((double)shape.x, 0.0, (double)i);
        this->push(1.0, 0.0, temp);

        this->push((double)shape.x, (double)shape.y, (double)i);
        this->push(1.0, 1.0, temp);

        // triangle
        this->push(0.0, 0.0, (double)i);
        this->push(0.0, 0.0, temp);

        this->push((double)shape.x, (double)shape.y, (double)i);
        this->push(1.0, 1.0, temp);

        this->push(0.0, (double)shape.y, (double)i);
        this->push(0.0, 1.0, temp);
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
    return glm::ivec3(this->_texture_1d.size(), 0, 0);
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