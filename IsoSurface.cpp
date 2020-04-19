#include "IsoSurface.h"

#include "constant.h"

#include <iostream>
#include <algorithm>
#include <iomanip>

using namespace std;

IsoSurface::IsoSurface(string inf_file, string raw_file) : super::Method(inf_file, raw_file)
{
    this->iso_value = super::volume.average();
}

IsoSurface::IsoSurface(string inf_file, string raw_file, float iso_value) : super::Method(inf_file, raw_file)
{
    this->iso_value = iso_value;
}

IsoSurface::~IsoSurface()
{

}

pair<glm::vec3, glm::vec3> IsoSurface::interpolation(glm::ivec3 p1, glm::ivec3 p2)
{
    glm::vec3 voxel_size = super::volume.voxel_size();

    double v1 = super::volume(p1).first;
    double v2 = super::volume(p2).first;
    glm::vec3 n1 = super::volume(p1).second;
    glm::vec3 n2 = super::volume(p2).second;

    if (glm::length(n1) >= EPSILON) n1 = glm::normalize(n1);
    if (glm::length(n2) >= EPSILON) n2 = glm::normalize(n2);

    if (fabs(this->iso_value - v1) < EPSILON) return make_pair(glm::vec3(p1) * voxel_size, n1);
    if (fabs(this->iso_value - v2) < EPSILON) return make_pair(glm::vec3(p2) * voxel_size, n2);
    if (fabs(v1 - v2) < EPSILON) return make_pair(glm::vec3(p1) * voxel_size, n1);

    double mu;
    glm::vec3 coordinate_temp, normal_temp;

    mu = (this->iso_value - v1) / (v2 - v1);

    coordinate_temp = glm::vec3(p1) + (float)mu * (glm::vec3(p2) - glm::vec3(p1));
    normal_temp = n1 + (float)mu * (n2 - n1);

    if (glm::length(normal_temp) >= EPSILON) normal_temp = glm::normalize(normal_temp);

    return make_pair(coordinate_temp * voxel_size, normal_temp);
}

void IsoSurface::run()
{
    super::volume.show();
    cout << "iso value: " << this->iso_value << '\n';

    vector<pair<glm::vec3, glm::vec3>> v(12);

    this->_vertex.clear();
    this->_vertex.shrink_to_fit();

    for (auto i = 0; i < super::volume.shape().x - 1; i++) {
        for (auto j = 0; j < super::volume.shape().y - 1; j++) {
            for (auto k = 0; k < super::volume.shape().z - 1; k++) {
                int index = 0;
                if (super::volume(i, j, k).first < this->iso_value) index |= 1;
                if (super::volume(i, j, k + 1).first < this->iso_value) index |= 2;
                if (super::volume(i, j + 1, k + 1).first < this->iso_value) index |= 4;
                if (super::volume(i, j + 1, k).first < this->iso_value) index |= 8;
                if (super::volume(i + 1, j, k).first < this->iso_value) index |= 16;
                if (super::volume(i + 1, j, k + 1).first < this->iso_value) index |= 32;
                if (super::volume(i + 1, j + 1, k + 1).first < this->iso_value) index |= 64;
                if (super::volume(i + 1, j + 1, k).first < this->iso_value) index |= 128;

                if (edgeTable[index] == 0) continue;
                if (edgeTable[index] & 1) v[0] = this->interpolation(glm::ivec3(i, j, k), glm::ivec3(i, j, k + 1));
                if (edgeTable[index] & 2) v[1] = this->interpolation(glm::ivec3(i, j, k + 1), glm::ivec3(i, j + 1, k + 1));
                if (edgeTable[index] & 4) v[2] = this->interpolation(glm::ivec3(i, j + 1, k + 1), glm::ivec3(i, j + 1, k));
                if (edgeTable[index] & 8) v[3] = this->interpolation(glm::ivec3(i, j, k), glm::ivec3(i, j + 1, k));
                if (edgeTable[index] & 16) v[4] = this->interpolation(glm::ivec3(i + 1, j, k), glm::ivec3(i + 1, j, k + 1));
                if (edgeTable[index] & 32) v[5] = this->interpolation(glm::ivec3(i + 1, j, k + 1), glm::ivec3(i + 1, j + 1, k + 1));
                if (edgeTable[index] & 64) v[6] = this->interpolation(glm::ivec3(i + 1, j + 1, k + 1), glm::ivec3(i + 1, j + 1, k));
                if (edgeTable[index] & 128) v[7] = this->interpolation(glm::ivec3(i + 1, j, k), glm::ivec3(i + 1, j + 1, k));
                if (edgeTable[index] & 256) v[8] = this->interpolation(glm::ivec3(i, j, k), glm::ivec3(i + 1, j, k));
                if (edgeTable[index] & 512) v[9] = this->interpolation(glm::ivec3(i, j, k + 1), glm::ivec3(i + 1, j, k + 1));
                if (edgeTable[index] & 1024) v[10] = this->interpolation(glm::ivec3(i, j + 1, k + 1), glm::ivec3(i + 1, j + 1, k + 1));
                if (edgeTable[index] & 2048) v[11] = this->interpolation(glm::ivec3(i, j + 1, k), glm::ivec3(i + 1, j + 1, k));

                for (auto vertex = 0; triangleTable[index][vertex] != -1 && vertex < 16; vertex += 3) {
                    for (auto delta_vertex = 0; delta_vertex < 3; delta_vertex++) {
                        this->_vertex.push_back(v[triangleTable[index][vertex + delta_vertex]].first.x);
                        this->_vertex.push_back(v[triangleTable[index][vertex + delta_vertex]].first.y);
                        this->_vertex.push_back(v[triangleTable[index][vertex + delta_vertex]].first.z);

                        this->_vertex.push_back(v[triangleTable[index][vertex + delta_vertex]].second.x);
                        this->_vertex.push_back(v[triangleTable[index][vertex + delta_vertex]].second.y);
                        this->_vertex.push_back(v[triangleTable[index][vertex + delta_vertex]].second.z);
                    }
                }
            }
        }
    }

    cout << "vertex size: " << this->_vertex.size() << '\n';
}

vector<GLfloat>& IsoSurface::vertex()
{
    return this->_vertex;
}

vector<int> IsoSurface::attribute()
{
    return vector<int>{3, 3};
}

GLenum IsoSurface::render_mode()
{
    return GL_TRIANGLES;
}