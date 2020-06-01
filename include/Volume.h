#pragma once

#include "constant.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cstring>
#include <iomanip>
#include <algorithm>

using namespace std;

class Volume {
private:
    int byte_size;
    double _max_value, _min_value;
    string inf_file, raw_file;
    TYPE _type;
    ENDIAN _endian, machine_endian;
    glm::ivec3 _shape;
    glm::vec3 _voxel_size;
    vector<vector<vector<pair<float, glm::vec3>>>> data;

    template <typename T>
    T handle_endian(int index, char *temp)
    {
        T value;

        if (this->machine_endian != this->_endian) reverse(temp + index, temp + index + this->byte_size);
        memcpy(&value, temp + index, this->byte_size);

        return value;
    }

    glm::vec3 gradient(int x, int y, int z);

    void load_inf();
    void load_raw();
    void calculate();

public:
    Volume();
    Volume(string inf_file, string raw_file);
    ~Volume();

    vector<int> histogram();
    vector<vector<int>> distribution_table(double gradient_max);
    double average();
    glm::vec2 limit_value();
    glm::ivec3 shape();
    glm::vec3 voxel_size();
    void show();

    pair<float, glm::vec3> operator()(int x, int y, int z);
    pair<float, glm::vec3> operator()(glm::ivec3 index);
};