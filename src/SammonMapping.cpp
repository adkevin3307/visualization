#include "SammonMapping.h"

#include "constant.h"

#include <iostream>
#include <cmath>
#include <random>
#include <functional>
#include <fstream>
#include <algorithm>

using namespace std;

SammonMapping::SammonMapping()
{

}

SammonMapping::SammonMapping(string filename)
{
    this->load_custom(filename);
}

SammonMapping::SammonMapping(string inf_file, string raw_file) : super::Method(inf_file, raw_file)
{
    this->load_scalar();
}

SammonMapping::~SammonMapping()
{

}

float SammonMapping::distance(vector<float> p1, vector<float> p2)
{
    float result = 0;

    for (size_t i = 0; i < p1.size(); i++) {
        result += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    }

    result = sqrt(result);

    return (result < EPSILON ? EPSILON : result);
}

void SammonMapping::normalize()
{
    vector<float> mu(this->data[0].size(), 0.0), sigma(this->data[0].size(), 0.0);
    // accmulate mu
    for (size_t i = 0; i < this->data.size(); i++) {
        for (size_t j = 0; j < this->data[i].size(); j++) {
            mu[j] += this->data[i][j];
        }
    }
    // calculate mu
    for (size_t i = 0; i < mu.size(); i++) {
        mu[i] /= (float)this->data.size();
    }
    // accmulate sigma
    for (size_t i = 0; i < this->data.size(); i++) {
        for (size_t j = 0; j < this->data[i].size(); j++) {
            sigma[j] += (this->data[i][j] - mu[j]) * (this->data[i][j] - mu[j]);
        }
    }
    // calculate sigma
    for (size_t i = 0; i < sigma.size(); i++) {
        sigma[i] = sqrt(sigma[i] / (float)this->data.size());
    }
    // normalize
    for (size_t i = 0; i < this->data.size(); i++) {
        for (size_t j = 0; j < this->data[i].size(); j++) {
            this->data[i][j] = (this->data[i][j] - mu[j]) / sigma[j];
        }
    }
}

void SammonMapping::kmeans(int group)
{
    this->group = group;

    this->label.resize(this->data.size(), 0);

    random_device device;
    default_random_engine generator = default_random_engine(device());

    vector<vector<float>> group_center(group, vector<float>(this->data[0].size()));
    sample(this->data.begin(), this->data.end(), group_center.begin(), this->group, generator);

    for (auto times = 0; times < 1000; times++) {
        // divide group
        for (size_t i = 0; i < this->data.size(); i++) {
            int min_index;
            float min_distance;

            for (size_t j = 0; j < group_center.size(); j++) {
                float temp = this->distance(group_center[j], this->data[i]);

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

void SammonMapping::calculate_distance()
{
    if (this->data.size() >= 1000) return;

    this->_distance.resize(this->data.size(), vector<float>(this->data.size(), EPSILON));

    for (size_t i = 0; i < this->data.size(); i++) {
        for (size_t j = i + 1; j < this->data.size(); j++) {
            this->_distance[i][j] = this->_distance[j][i] = this->distance(this->data[i], this->data[j]);
        }
    }

    cout << "distance table size: " << this->_distance.size() << ", " << this->_distance[0].size() << '\n';
}

void SammonMapping::load_custom(string filename)
{
    fstream file;
    file.open(filename, ios::in);

    int x, y;
    file >> x >> y;

    this->data.resize(x, vector<float>(y, 0.0));

    for (size_t i = 0; i < this->data.size(); i++) {
        for (size_t j = 0; j < this->data[i].size(); j++) {
            file >> this->data[i][j];
        }
    }

    file.close();

    cout << "data size: " << this->data.size() << ", " << this->data[0].size() << '\n';

    this->normalize();
    this->kmeans(3);
    this->calculate_distance();
}

void SammonMapping::load_scalar()
{
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
                    float gradient_magnitude = glm::length(super::volume(i, j, k).second);
                    glm::vec3 position = glm::vec3(i, j, k) * super::volume.voxel_size();

                    temp.push_back(value);
                    temp.push_back(gradient_magnitude);
                    for (auto delta = 0; delta < 3; delta++) {
                        temp.push_back(position[delta]);
                    }
                    
                    this->data.push_back(temp);
                }
            }
        }
    }

    cout << "data size: " << this->data.size() << ", " << this->data[0].size() << '\n';

    this->normalize();
    this->kmeans(3);
    this->calculate_distance();
}

glm::vec2 SammonMapping::descent(float lambda, glm::ivec2 index, vector<glm::vec2> &mapping_point)
{
    float true_distance;
    if (this->_distance.size() != 0) true_distance = this->_distance[index.x][index.y];
    else true_distance = this->distance(this->data[index.x], this->data[index.y]);

    float mapping_distance = this->distance(
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

    for (size_t i = 0; i < mapping_point.size(); i++) {
        mapping_point[i].x = random();
        mapping_point[i].y = random();
    }

    vector<glm::vec3> color;
    color.push_back(glm::vec3(1.0, 0.0, 0.0));
    color.push_back(glm::vec3(0.0, 1.0, 0.0));
    color.push_back(glm::vec3(0.0, 0.0, 1.0));
    color.push_back(glm::vec3(1.0, 1.0, 0.0));
    color.push_back(glm::vec3(1.0, 0.0, 1.0));
    color.push_back(glm::vec3(0.0, 1.0, 1.0));

    for (auto i = (int)color.size(); i < this->group; i++) {
        color.push_back(glm::vec3(
            min(1.0, (random() + 1.0) / 2.0 + 0.3),
            min(1.0, (random() + 1.0) / 2.0 + 0.3),
            min(1.0, (random() + 1.0) / 2.0 + 0.3)
        ));
    }

    int times;
    for (times = 0; times < 10000; times++) {
        float sum = 0;
        for (size_t i = 0; i < mapping_point.size(); i++) {
            for (size_t j = 0; j < mapping_point.size(); j++) {
                if (j == i) continue;

                glm::vec2 delta = this->descent(lambda, glm::ivec2(i, j), mapping_point);

                mapping_point[i] += delta;
                mapping_point[j] -= delta;

                sum += glm::length(delta) * 2;
            }
        }
        lambda *= alpha;

        if (sum < EPSILON) {
            times += 1;
            break;
        }
    }

    cout << "times: " << times << '\n';

    for (size_t i = 0; i < mapping_point.size(); i++) {
        this->_vertex.push_back(mapping_point[i].x);
        this->_vertex.push_back(mapping_point[i].y);

        this->_vertex.push_back(color[this->label[i]].x);
        this->_vertex.push_back(color[this->label[i]].y);
        this->_vertex.push_back(color[this->label[i]].z);
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
    return vector<int>{2, 3};
}

GLenum SammonMapping::render_mode()
{
    return GL_POINTS;
}