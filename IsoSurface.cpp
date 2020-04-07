#include "IsoSurface.h"

#include "include/constant.h"

#include <iostream>
#include <algorithm>

using namespace std;

IsoSurface::IsoSurface()
{

}

IsoSurface::IsoSurface(string inf_file, string raw_file)
{
    this->volume = Volume(inf_file, raw_file);
    this->iso_value = this->volume.average();

    this->volume.show();
}

IsoSurface::~IsoSurface()
{

}

pair<glm::vec3, glm::vec3> IsoSurface::interpolation(glm::ivec3 p1, glm::ivec3 p2)
{
    glm::vec3 voxel_size = this->volume.voxel_size();

    double v1 = this->volume(p1).first;
    double v2 = this->volume(p2).first;
    glm::vec3 n1 = this->volume(p1).second;
    glm::vec3 n2 = this->volume(p2).second;

    if (glm::length(n1) >= EPSILON) n1 = glm::normalize(n1);
    if (glm::length(n2) >= EPSILON) n2 = glm::normalize(n2);

    if (fabs(this->iso_value - v1) < EPSILON) return make_pair(glm::vec3(p1) * voxel_size, n1);
    if (fabs(this->iso_value - v2) < EPSILON) return make_pair(glm::vec3(p2) * voxel_size, n2);
    if (fabs(v1 - v2) < EPSILON) return make_pair(glm::vec3(p1) * voxel_size, n1);

    double mu;
    glm::vec3 coordinate_temp, normal_temp;

    mu = (this->iso_value - v1) / (v2 - v1);

    coordinate_temp.x = p1.x + mu * (p2.x - p1.x);
    coordinate_temp.y = p1.y + mu * (p2.y - p1.y);
    coordinate_temp.z = p1.z + mu * (p2.z - p1.z);

    normal_temp = n1 + (float)mu * (n2 - n1);

    if (glm::length(normal_temp) >= EPSILON) normal_temp = glm::normalize(normal_temp);

    return make_pair(coordinate_temp * voxel_size, normal_temp);
}

void IsoSurface::calculate()
{
    vector<pair<glm::vec3, glm::vec3>> v(12);

    this->_vertex.clear();
    this->_vertex.shrink_to_fit();

    for (auto i = 0; i < this->volume.shape().x - 1; i++) {
        for (auto j = 0; j < this->volume.shape().y - 1; j++) {
            for (auto k = 0; k < this->volume.shape().z - 1; k++) {
                int index = 0;
                if (this->volume(i, j, k).first < this->iso_value) index |= 1;
                if (this->volume(i, j, k + 1).first < this->iso_value) index |= 2;
                if (this->volume(i, j + 1, k + 1).first < this->iso_value) index |= 4;
                if (this->volume(i, j + 1, k).first < this->iso_value) index |= 8;
                if (this->volume(i + 1, j, k).first < this->iso_value) index |= 16;
                if (this->volume(i + 1, j, k + 1).first < this->iso_value) index |= 32;
                if (this->volume(i + 1, j + 1, k + 1).first < this->iso_value) index |= 64;
                if (this->volume(i + 1, j + 1, k).first < this->iso_value) index |= 128;

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

    cout << "iso value: " << this->iso_value << '\n';
    cout << "vertex size: " << this->_vertex.size() << '\n';
}

void IsoSurface::run()
{
    this->calculate();
}

void IsoSurface::run(float value)
{
    this->iso_value = value;

    this->calculate();
}

vector<GLfloat>& IsoSurface::vertex()
{
    return this->_vertex;
}

glm::vec3 IsoSurface::volume_shape()
{
    return glm::vec3(this->volume.shape()) * this->volume.voxel_size();
}