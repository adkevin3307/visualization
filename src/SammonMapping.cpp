#include "SammonMapping.h"

#include "constant.h"

#include <iostream>
#include <cmath>
#include <random>
#include <functional>

using namespace std;

SammonMapping::SammonMapping()
{
    this->load_data();
}

SammonMapping::SammonMapping(string inf_file, string raw_file) : super::Method(inf_file, raw_file)
{
    this->load_data();
}

SammonMapping::~SammonMapping()
{

}

float SammonMapping::norm_2(vector<float> p1, vector<float> p2)
{
    float result = 0;

    for (size_t i = 0; i < p1.size(); i++) {
        result += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    }

    result = sqrt(result);

    return (result < EPSILON ? EPSILON : result);
}

void SammonMapping::kmeans(int group)
{
    this->group = group;

    this->label.resize(this->data.size(), 0);

    random_device device;
    default_random_engine generator = default_random_engine(device());
    uniform_real_distribution<int> distribution(0, this->data.size() - 1);
    auto random = bind(distribution, generator);

    vector<vector<float>> group_center(group, vector<float>(this->data[0].size()));
    for (size_t i = 0; i < group_center.size(); i++) {
        group_center[i] = this->data[random()];
    }

    for (auto times = 0; times < 1000; times++) {
        // divide group
        for (size_t i = 0; i < this->data.size(); i++) {
            int min_index;
            float min_distance;

            for (size_t j = 0; j < group_center.size(); j++) {
                float temp = this->norm_2(group_center[j], this->data[i]);

                if (j == 0 || temp < min_distance) {
                    min_distance = temp;
                    min_index = j;
                }
            }
            this->label[i] = min_index;
        }
        // update group center
        int flag = 0;
        for (size_t i = 0; i < group_center.size(); i++) {
            int count = 0;
            vector<float> sum(group_center[i].size(), 0.0);
            // accumulate
            for (size_t j = 0; j < this->data.size(); j++) {
                if (this->label[j] == (int)i) {
                    count += 1;

                    for (size_t k = 0; k < sum.size(); k++) {
                        sum[k] += this->data[j][k];
                    }
                }
            }
            // update
            for (size_t j = 0; j < group_center[i].size(); j++) {
                float result = sum[j] / (float)count;
                if (fabs(result - group_center[i][j]) < EPSILON) flag += 1;

                group_center[i][j] = result;
            }
        }
        if (flag >= (int)(this->data.size() * this->data[0].size() - 10)) break;
    }
}

void SammonMapping::load_data()
{
    // this->data = vector<vector<float>>{
    //     { 90, 50, 55, 90, 90, 72, 56, 57, 62 },
    //     { 87, 77, 90, 87, 90, 43, 37, 42, 12 },
    //     { 90, 50, 0, 0, 0, 49, 47, 63, 13 },
    //     { 90, 94, 90, 100, 87, 58, 75, 53, 50 },
    //     { 90, 67, 45, 77, 0, 45, 64, 32, 32 },
    //     { 70, 0, 0, 0, 0, 62, 57, 22, 38 },
    //     { 95, 87, 90, 90, 92, 49, 50, 66, 35 },
    //     { 95, 72, 85, 87, 90, 64, 73, 60, 68 },
    //     { 95, 90, 60, 92, 87, 0, 0, 79, 38 },
    //     { 88, 69, 90, 90, 87, 88, 96, 75, 64 },
    //     { 0, 30, 50, 85, 90, 18, 49, 35, 28 },
    //     { 0, 0, 0, 0, 0, 19, 27, 31, 18 },
    //     { 94, 108, 90, 90, 90, 88, 60, 60, 70 },
    //     { 92, 78, 90, 87, 87, 79, 70, 35, 71 },
    //     { 96, 99, 90, 90, 90, 76, 64, 71, 76 },
    //     { 100, 100, 90, 95, 100, 88, 90, 81, 90 },
    //     { 88, 0, 0, 0, 0, 22, 0, 14, 12 },
    //     { 87, 64, 60, 77, 90, 75, 95, 63, 78 },
    //     { 87, 30, 40, 65, 85, 81, 60, 38, 77 },
    //     { 0, 0, 0, 0, 0, 31, 31, 9, 0 },
    //     { 93, 72, 90, 90, 90, 64, 44, 42, 29 },
    //     { 87, 70, 60, 77, 87, 69, 49, 47, 58 },
    //     { 88, 67, 70, 0, 90, 51, 37, 36, 35 },
    //     { 88, 80, 85, 87, 87, 66, 46, 36, 38 },
    //     { 70, 35, 0, 0, 45, 53, 39, 45, 29 },
    //     { 20, 0, 35, 50, 67, 34, 37, 25, 56 },
    //     { 60, 50, 55, 45, 60, 55, 50, 60, 60 },
    //     { 61, 45, 50, 50, 55, 60, 50, 58, 61 },
    //     { 10, 20, 0, 0, 0, 34, 45, 22, 40 },
    //     { 30, 40, 40, 10, 70, 70, 60, 60, 70 }
    // };

    // this->data = vector<vector<float>>{
    //     { 5.1, 3.5, 1.4, 0.2 },
    //     { 4.9, 3.0, 1.4, 0.2 },
    //     { 4.7, 3.2, 1.3, 0.2 },
    //     { 4.6, 3.1, 1.5, 0.2 },
    //     { 5.0, 3.6, 1.4, 0.2 },
    //     { 5.4, 3.9, 1.7, 0.4 },
    //     { 4.6, 3.4, 1.4, 0.3 },
    //     { 5.0, 3.4, 1.5, 0.2 },
    //     { 4.4, 2.9, 1.4, 0.2 },
    //     { 4.9, 3.1, 1.5, 0.1 },
    //     { 5.4, 3.7, 1.5, 0.2 },
    //     { 4.8, 3.4, 1.6, 0.2 },
    //     { 4.8, 3.0, 1.4, 0.1 },
    //     { 4.3, 3.0, 1.1, 0.1 },
    //     { 5.8, 4.0, 1.2, 0.2 },
    //     { 5.7, 4.4, 1.5, 0.4 },
    //     { 5.4, 3.9, 1.3, 0.4 },
    //     { 5.1, 3.5, 1.4, 0.3 },
    //     { 5.7, 3.8, 1.7, 0.3 },
    //     { 5.1, 3.8, 1.5, 0.3 },
    //     { 5.4, 3.4, 1.7, 0.2 },
    //     { 5.1, 3.7, 1.5, 0.4 },
    //     { 4.6, 3.6, 1.0, 0.2 },
    //     { 5.1, 3.3, 1.7, 0.5 },
    //     { 4.8, 3.4, 1.9, 0.2 },
    //     { 5.0, 3.0, 1.6, 0.2 },
    //     { 5.0, 3.4, 1.6, 0.4 },
    //     { 5.2, 3.5, 1.5, 0.2 },
    //     { 5.2, 3.4, 1.4, 0.2 },
    //     { 4.7, 3.2, 1.6, 0.2 },
    //     { 4.8, 3.1, 1.6, 0.2 },
    //     { 5.4, 3.4, 1.5, 0.4 },
    //     { 5.2, 4.1, 1.5, 0.1 },
    //     { 5.5, 4.2, 1.4, 0.2 },
    //     { 4.9, 3.1, 1.5, 0.1 },
    //     { 5.0, 3.2, 1.2, 0.2 },
    //     { 5.5, 3.5, 1.3, 0.2 },
    //     { 4.9, 3.1, 1.5, 0.1 },
    //     { 4.4, 3.0, 1.3, 0.2 },
    //     { 5.1, 3.4, 1.5, 0.2 },
    //     { 5.0, 3.5, 1.3, 0.3 },
    //     { 4.5, 2.3, 1.3, 0.3 },
    //     { 4.4, 3.2, 1.3, 0.2 },
    //     { 5.0, 3.5, 1.6, 0.6 },
    //     { 5.1, 3.8, 1.9, 0.4 },
    //     { 4.8, 3.0, 1.4, 0.3 },
    //     { 5.1, 3.8, 1.6, 0.2 },
    //     { 4.6, 3.2, 1.4, 0.2 },
    //     { 5.3, 3.7, 1.5, 0.2 },
    //     { 5.0, 3.3, 1.4, 0.2 },
    //     { 7.0, 3.2, 4.7, 1.4 },
    //     { 6.4, 3.2, 4.5, 1.5 },
    //     { 6.9, 3.1, 4.9, 1.5 },
    //     { 5.5, 2.3, 4.0, 1.3 },
    //     { 6.5, 2.8, 4.6, 1.5 },
    //     { 5.7, 2.8, 4.5, 1.3 },
    //     { 6.3, 3.3, 4.7, 1.6 },
    //     { 4.9, 2.4, 3.3, 1.0 },
    //     { 6.6, 2.9, 4.6, 1.3 },
    //     { 5.2, 2.7, 3.9, 1.4 },
    //     { 5.0, 2.0, 3.5, 1.0 },
    //     { 5.9, 3.0, 4.2, 1.5 },
    //     { 6.0, 2.2, 4.0, 1.0 },
    //     { 6.1, 2.9, 4.7, 1.4 },
    //     { 5.6, 2.9, 3.6, 1.3 },
    //     { 6.7, 3.1, 4.4, 1.4 },
    //     { 5.6, 3.0, 4.5, 1.5 },
    //     { 5.8, 2.7, 4.1, 1.0 },
    //     { 6.2, 2.2, 4.5, 1.5 },
    //     { 5.6, 2.5, 3.9, 1.1 },
    //     { 5.9, 3.2, 4.8, 1.8 },
    //     { 6.1, 2.8, 4.0, 1.3 },
    //     { 6.3, 2.5, 4.9, 1.5 },
    //     { 6.1, 2.8, 4.7, 1.2 },
    //     { 6.4, 2.9, 4.3, 1.3 },
    //     { 6.6, 3.0, 4.4, 1.4 },
    //     { 6.8, 2.8, 4.8, 1.4 },
    //     { 6.7, 3.0, 5.0, 1.7 },
    //     { 6.0, 2.9, 4.5, 1.5 },
    //     { 5.7, 2.6, 3.5, 1.0 },
    //     { 5.5, 2.4, 3.8, 1.1 },
    //     { 5.5, 2.4, 3.7, 1.0 },
    //     { 5.8, 2.7, 3.9, 1.2 },
    //     { 6.0, 2.7, 5.1, 1.6 },
    //     { 5.4, 3.0, 4.5, 1.5 },
    //     { 6.0, 3.4, 4.5, 1.6 },
    //     { 6.7, 3.1, 4.7, 1.5 },
    //     { 6.3, 2.3, 4.4, 1.3 },
    //     { 5.6, 3.0, 4.1, 1.3 },
    //     { 5.5, 2.5, 4.0, 1.3 },
    //     { 5.5, 2.6, 4.4, 1.2 },
    //     { 6.1, 3.0, 4.6, 1.4 },
    //     { 5.8, 2.6, 4.0, 1.2 },
    //     { 5.0, 2.3, 3.3, 1.0 },
    //     { 5.6, 2.7, 4.2, 1.3 },
    //     { 5.7, 3.0, 4.2, 1.2 },
    //     { 5.7, 2.9, 4.2, 1.3 },
    //     { 6.2, 2.9, 4.3, 1.3 },
    //     { 5.1, 2.5, 3.0, 1.1 },
    //     { 5.7, 2.8, 4.1, 1.3 },
    //     { 6.3, 3.3, 6.0, 2.5 },
    //     { 5.8, 2.7, 5.1, 1.9 },
    //     { 7.1, 3.0, 5.9, 2.1 },
    //     { 6.3, 2.9, 5.6, 1.8 },
    //     { 6.5, 3.0, 5.8, 2.2 },
    //     { 7.6, 3.0, 6.6, 2.1 },
    //     { 4.9, 2.5, 4.5, 1.7 },
    //     { 7.3, 2.9, 6.3, 1.8 },
    //     { 6.7, 2.5, 5.8, 1.8 },
    //     { 7.2, 3.6, 6.1, 2.5 },
    //     { 6.5, 3.2, 5.1, 2.0 },
    //     { 6.4, 2.7, 5.3, 1.9 },
    //     { 6.8, 3.0, 5.5, 2.1 },
    //     { 5.7, 2.5, 5.0, 2.0 },
    //     { 5.8, 2.8, 5.1, 2.4 },
    //     { 6.4, 3.2, 5.3, 2.3 },
    //     { 6.5, 3.0, 5.5, 1.8 },
    //     { 7.7, 3.8, 6.7, 2.2 },
    //     { 7.7, 2.6, 6.9, 2.3 },
    //     { 6.0, 2.2, 5.0, 1.5 },
    //     { 6.9, 3.2, 5.7, 2.3 },
    //     { 5.6, 2.8, 4.9, 2.0 },
    //     { 7.7, 2.8, 6.7, 2.0 },
    //     { 6.3, 2.7, 4.9, 1.8 },
    //     { 6.7, 3.3, 5.7, 2.1 },
    //     { 7.2, 3.2, 6.0, 1.8 },
    //     { 6.2, 2.8, 4.8, 1.8 },
    //     { 6.1, 3.0, 4.9, 1.8 },
    //     { 6.4, 2.8, 5.6, 2.1 },
    //     { 7.2, 3.0, 5.8, 1.6 },
    //     { 7.4, 2.8, 6.1, 1.9 },
    //     { 7.9, 3.8, 6.4, 2.0 },
    //     { 6.4, 2.8, 5.6, 2.2 },
    //     { 6.3, 2.8, 5.1, 1.5 },
    //     { 6.1, 2.6, 5.6, 1.4 },
    //     { 7.7, 3.0, 6.1, 2.3 },
    //     { 6.3, 3.4, 5.6, 2.4 },
    //     { 6.4, 3.1, 5.5, 1.8 },
    //     { 6.0, 3.0, 4.8, 1.8 },
    //     { 6.9, 3.1, 5.4, 2.1 },
    //     { 6.7, 3.1, 5.6, 2.4 },
    //     { 6.9, 3.1, 5.1, 2.3 },
    //     { 5.8, 2.7, 5.1, 1.9 },
    //     { 6.8, 3.2, 5.9, 2.3 },
    //     { 6.7, 3.3, 5.7, 2.5 },
    //     { 6.7, 3.0, 5.2, 2.3 },
    //     { 6.3, 2.5, 5.0, 1.9 },
    //     { 6.5, 3.0, 5.2, 2.0 },
    //     { 6.2, 3.4, 5.4, 2.3 },
    //     { 5.9, 3.0, 5.1, 1.8 }
    // };

    random_device device;
    default_random_engine generator = default_random_engine(device());
    uniform_real_distribution<float> distribution(0, 10000);
    auto random = bind(distribution, generator);

    for (auto i = 0; i < super::volume.shape().x; i++) {
        for (auto j = 0; j < super::volume.shape().y; j++) {
            for (auto k = 0; k < super::volume.shape().z; k++) {
                if (random() < 1) {
                    vector<float> temp;

                    float value = super::volume(i, j, k).first;

                    glm::vec3 normal = super::volume(i, j, k).second;

                    temp.push_back(value);
                    for (auto delta = 0; delta < 3; delta++) {
                        temp.push_back(normal[delta]);
                    }
                    
                    this->data.push_back(temp);
                }
            }
        }
    }

    cout << "data size: " << this->data.size() << ", " << this->data[0].size() << '\n';

    this->kmeans(3);

    if (this->data.size() < 1000) {
        this->_distance.resize(this->data.size(), vector<float>(this->data.size(), EPSILON));

        for (size_t i = 0; i < this->data.size(); i++) {
            for (size_t j = i + 1; j < this->data.size(); j++) {
                this->_distance[i][j] = this->_distance[j][i] = this->norm_2(this->data[i], this->data[j]);
            }
        }

        cout << "distance table size: " << this->_distance.size() << ", " << this->_distance[0].size() << '\n';
    }
}

glm::vec2 SammonMapping::descent(float lambda, glm::ivec2 index, vector<glm::vec2> &mapping_point)
{
    float true_distance;
    if (this->_distance.size() != 0) true_distance = this->_distance[index.x][index.y];
    else true_distance = this->norm_2(this->data[index.x], this->data[index.y]);

    float mapping_distance = this->norm_2(
        vector<float>{mapping_point[index.x].x, mapping_point[index.x].y},
        vector<float>{mapping_point[index.y].x, mapping_point[index.y].y}
    );

    float coefficient = lambda * (true_distance - mapping_distance) / mapping_distance;

    return coefficient * (mapping_point[index.x] - mapping_point[index.y]);
}

void SammonMapping::run(float alpha)
{
    float lambda = 0.3;
    vector<glm::vec2> mapping_point(this->data.size(), glm::vec2(0.0));

    random_device device;
    default_random_engine generator = default_random_engine(device());

    uniform_real_distribution<float> distribution(-1, 1);
    auto random = bind(distribution, generator);

    uniform_real_distribution<float> color_distribution(0, 1);
    auto color_random = bind(color_distribution, generator);

    for (size_t i = 0; i < mapping_point.size(); i++) {
        mapping_point[i].x = random();
        mapping_point[i].y = random();
    }

    vector<glm::vec3> color(this->group, glm::vec3(0.0));
    for (size_t i = 0; i < color.size(); i++) {
        color[i].x = color_random();
        color[i].y = color_random();
        color[i].z = color_random();
    }

    for (auto times = 0; times < 500; times++) {
        for (size_t i = 0; i < mapping_point.size(); i++) {
            for (size_t j = 0; j < mapping_point.size(); j++) {
                if (j == i) continue;

                glm::vec2 delta = this->descent(lambda, glm::ivec2(i, j), mapping_point);

                mapping_point[i] += delta;
                mapping_point[j] -= delta;
            }
        }
        lambda *= alpha;
    }

    for (size_t i = 0; i < mapping_point.size(); i++) {
        this->_vertex.push_back(mapping_point[i].x);
        this->_vertex.push_back(mapping_point[i].y);

        this->_vertex.push_back(color[this->label[i]].x);
        this->_vertex.push_back(color[this->label[i]].y);
        this->_vertex.push_back(color[this->label[i]].z);
        this->_vertex.push_back(1.0);
    }

    cout << "vertex size: " << this->_vertex.size() << '\n';
}

glm::vec3 SammonMapping::shape()
{
    return glm::vec3(0.0);
}

vector<GLfloat>& SammonMapping::vertex()
{
    return this->_vertex;
}

vector<int> SammonMapping::attribute()
{
    return vector<int>{2, 4};
}

GLenum SammonMapping::render_mode()
{
    return GL_POINTS;
}