#include "Volume.h"

#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdexcept>
#include <cctype>
#include <map>
#include <numeric>

using namespace std;

Volume::Volume()
    : byte_size(0), inf_file(""), raw_file(""), _type(TYPE::CHAR), _endian(ENDIAN::LITTLE)
{
    this->_max_value = this->_min_value = 0.0;
    this->_shape.x = this->_shape.y = this->_shape.z = 0;
    this->_voxel_size.x = this->_voxel_size.y = this->_voxel_size.z = 1.0;

    short int word = 0x0001;
    char *check = (char*)&word;
    this->machine_endian = (check[0] ? ENDIAN::LITTLE : ENDIAN::BIG);
}

Volume::Volume(string inf_file, string raw_file) : Volume::Volume()
{
    this->inf_file = inf_file;
    this->raw_file = raw_file;

    if (access(inf_file.c_str(), F_OK) == -1) throw runtime_error(inf_file + " Not Exist");
    if (access(raw_file.c_str(), F_OK) == -1) throw runtime_error(raw_file + " Not Exist");

    this->load_inf();
    this->load_raw();
    this->calculate();
}

Volume::~Volume()
{
    this->data.clear();
    this->data.shrink_to_fit();
}

glm::vec3 Volume::gradient(int x, int y, int z)
{
    glm::vec3 result;

    glm::ivec3 largest(min(this->_shape.x - 1, x + 1), min(this->_shape.y - 1, y + 1), min(this->_shape.z - 1, z + 1));
    glm::ivec3 smallest(max(0, x - 1), max(0, y - 1), max(0, z - 1));
    glm::vec3 delta = (glm::vec3(largest) - glm::vec3(smallest)) * this->_voxel_size;

    result.x = (this->data[largest.x][y][z].first - this->data[smallest.x][y][z].first) / delta.x;
    result.y = (this->data[x][largest.y][z].first - this->data[x][smallest.y][z].first) / delta.y;
    result.z = (this->data[x][y][largest.z].first - this->data[x][y][smallest.z].first) / delta.z;

    return result;
}

void Volume::load_inf()
{
    string s;
    fstream file;

    file.open(this->inf_file, ios::in);

    while (getline(file, s)) {
        if (s[0] == '#') continue;

        s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
        transform(s.begin(), s.end(), s.begin(), ::tolower);

        string key = s.substr(0, s.find('=')), value = s.substr(s.find('=') + 1);

        if (key.find("file") != string::npos) continue;
        else if (key.find("endian") != string::npos || key.find("indian") != string::npos) {
            this->_endian = (value.find("l") != string::npos ? ENDIAN::LITTLE : ENDIAN::BIG);
        }
        else if (key.find("resolution") != string::npos) {
            replace(value.begin(), value.end(), 'x', ':');
            sscanf(value.c_str(), "%d:%d:%d", &(this->_shape.z), &(this->_shape.y), &(this->_shape.x));
        }
        else if (key.find("voxel") != string::npos || key.find("ratio") != string::npos) {
            replace(value.begin(), value.end(), 'x', ':');
            sscanf(value.c_str(), "%f:%f:%f", &(this->_voxel_size.z), &(this->_voxel_size.y), &(this->_voxel_size.x));
        }
        else if (key.find("sample") != string::npos || key.find("value") != string::npos) {
            if (value.find("f") != string::npos) {
                this->byte_size = 4;
                this->_type = TYPE::FLOAT;
            }
            else if (value.find("char") != string::npos || value == "b" || value == "ub") {
                this->byte_size = 1;
                this->_type = ((value.find("u") != string::npos) ? TYPE::UNSIGNED_CHAR : TYPE::CHAR);
            }
            else if (value.find("short") != string::npos || value == "s" || value == "us") {
                this->byte_size = 2;
                this->_type = ((value.find("u") != string::npos) ? TYPE::UNSIGNED_SHORT : TYPE::SHORT);
            }
            else if (value.find("int") != string::npos || value == "i" || value == "ui") {
                this->byte_size = 4;
                this->_type = ((value.find("u") != string::npos) ? TYPE::UNSIGNED_INT : TYPE::INT);
            }
        }
    }
    file.close();

    this->data.resize(
        this->_shape.x, vector<vector<pair<float, glm::vec3>>>(
            this->_shape.y, vector<pair<float, glm::vec3>>(this->_shape.z)
        )
    );
}

void Volume::load_raw()
{
    fstream file;
    char *temp = new char[this->_shape.x * this->_shape.y * this->_shape.z * this->byte_size];
    
    file.open(this->raw_file, ios::in | ios::binary);
    file.read(temp, this->_shape.x * this->_shape.y * this->_shape.z * this->byte_size);
    file.close();

    for (auto i = 0; i < this->_shape.x; i++) {
        for (auto j = 0; j < this->_shape.y; j++) {
            for (auto k = 0; k < this->_shape.z; k++) {
                int index = (i * this->_shape.y * this->_shape.z + j * this->_shape.z + k) * this->byte_size;

                switch (this->_type) {
                    case TYPE::FLOAT:
                        this->data[i][j][k].first = this->handle_endian<float>(index, temp);
                        break;
                    case TYPE::UNSIGNED_CHAR:
                        this->data[i][j][k].first = (float)this->handle_endian<unsigned char>(index, temp);
                        break;
                    case TYPE::CHAR:
                        this->data[i][j][k].first = (float)this->handle_endian<char>(index, temp);
                        break;
                    case TYPE::UNSIGNED_SHORT:
                        this->data[i][j][k].first = (float)this->handle_endian<unsigned short>(index, temp);
                        break;
                    case TYPE::SHORT:
                        this->data[i][j][k].first = (float)this->handle_endian<short>(index, temp);
                        break;
                    case TYPE::UNSIGNED_INT:
                        this->data[i][j][k].first = (float)this->handle_endian<unsigned int>(index, temp);
                        break;
                    case TYPE::INT:
                        this->data[i][j][k].first = (float)this->handle_endian<int>(index, temp);
                        break;
                    default:
                        break;
                }

                if (i == 0 && j == 0 && k == 0) {
                    this->_max_value = this->_min_value = this->data[i][j][k].first;
                }
                if (this->data[i][j][k].first - this->_max_value > EPSILON) this->_max_value = this->data[i][j][k].first;
                if (this->_min_value - this->data[i][j][k].first > EPSILON) this->_min_value = this->data[i][j][k].first;
            }
        }
    }

    delete[] temp;
}

void Volume::calculate()
{
    for (auto i = 0; i < this->_shape.x; i++) {
        for (auto j = 0; j < this->_shape.y; j++) {
            for (auto k = 0; k < this->_shape.z; k++) {
                this->data[i][j][k].second = this->gradient(i, j, k);
            }
        }
    }
}

void Volume::equalization()
{
    vector<float> result = this->histogram();
    vector<float> pdf(256, 0.0), cdf(256, 0.0);

    double sum = accumulate(result.begin(), result.end(), 0.0);

    for (size_t i = 0; i < pdf.size(); i++) {
        pdf[i] = result[i] / sum;
    }

    for (size_t i = 0; i < cdf.size(); i++) {
        if (i == 0) cdf[i] = pdf[i];
        else cdf[i] = cdf[i - 1] + pdf[i];
    }

    map<int, int> index_map;
    for (size_t i = 0; i < cdf.size(); i++) {
        index_map[i] = (int)round(cdf[i] * (cdf.size() - 1));
    }

    double temp = 255.0 / (this->_max_value - this->_min_value);
    for (auto i = 0; i < this->_shape.x; i++) {
        for (auto j = 0; j < this->_shape.y; j++) {
            for (auto k = 0; k < this->_shape.z; k++) {
                float value = this->data[i][j][k].first;

                this->data[i][j][k].first = index_map[(value - this->_min_value) * temp];
            }
        }
    }

    this->_min_value = 0;
    this->_max_value = 255;

    this->calculate();
}

vector<float> Volume::histogram()
{
    vector<float> result;
    result.resize(256, 0.0);

    double temp = 255.0 / (this->_max_value - this->_min_value);

    for (auto i = 0; i < this->_shape.x; i++) {
        for (auto j = 0; j < this->_shape.y; j++) {
            for (auto k = 0; k < this->_shape.z; k++) {
                int index = (this->data[i][j][k].first - this->_min_value) * temp;
                result[index]++;
            }
        }
    }

    return result;
}

vector<vector<float>> Volume::distribution(double gradient_max)
{
    int size = (20 * log2(gradient_max)) + 1;

    vector<vector<float>> result;
    result.resize(256, vector<float>(size, 0.0));

    double temp = 255.0 / (this->_max_value - this->_min_value);

    for (auto i = 0; i < this->_shape.x; i++) {
        for (auto j = 0; j < this->_shape.y; j++) {
            for (auto k = 0; k < this->_shape.z; k++) {
                double magnitude = glm::length(this->data[i][j][k].second);
                magnitude = ::clamp(magnitude, 1.0, gradient_max);
                magnitude = 20 * log2(magnitude);

                int index_x = (this->data[i][j][k].first - this->_min_value) * temp;
                int index_y = magnitude;

                result[index_x][index_y]++;
            }
        }
    }

    return result;
}

double Volume::average()
{
    return (this->_max_value + this->_min_value) / 2.0;
}

glm::vec2 Volume::limit_value()
{
    return glm::vec2(this->_min_value, this->_max_value);
}

glm::ivec3 Volume::shape()
{
    return this->_shape;
}

glm::vec3 Volume::voxel_size()
{
    return this->_voxel_size;
}

void Volume::show()
{
    map<TYPE, string> m_type = {
        { TYPE::UNSIGNED_CHAR, "unsigned char" },
        { TYPE::CHAR, "char" },
        { TYPE::UNSIGNED_SHORT, "unsigned short" },
        { TYPE::SHORT, "short" },
        { TYPE::UNSIGNED_INT, "unsigned int" },
        { TYPE::INT, "int" },
        { TYPE::FLOAT, "float" }
    };
    map<ENDIAN, string> m_endian = {
        { ENDIAN::LITTLE, "little" },
        { ENDIAN::BIG, "big" }
    };

    cout << "inf file: " << this->inf_file << '\n';
    cout << "raw file: " << this->raw_file << '\n';
    cout << "endian: " << m_endian[this->_endian] << '\n';
    cout << "type: " << m_type[this->_type] << '\n';
    cout << "shape: " << glm::to_string(this->_shape) << '\n';
    cout << "min value: " << this->_min_value << '\n';
    cout << "max value: " << this->_max_value << '\n';
    cout << "voxel size: " << glm::to_string(this->_voxel_size) << '\n';
}

pair<float, glm::vec3> Volume::operator()(int x, int y, int z)
{
    return this->data[x][y][z];
}

pair<float, glm::vec3> Volume::operator()(glm::ivec3 index)
{
    return this->data[index.x][index.y][index.z];
}