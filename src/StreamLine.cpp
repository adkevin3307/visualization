#include "StreamLine.h"

#include "constant.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <io.h>

using namespace std;

StreamLine::StreamLine(string filename)
    : min_vector_magnitude(0.0), max_vector_magnitude(0.0)
{
    if (access(filename.c_str(), F_OK) == -1) throw runtime_error(filename + " Not Exist");

    this->load_data(filename);
}

StreamLine::~StreamLine()
{

}

void StreamLine::load_data(string filename)
{
    int x, y;

    fstream file;
    file.open(filename, ios::in);

    file >> x >> y;

    cout << "size: " << x << ", " << y << '\n';

    this->data.resize(y, vector<glm::vec2>(x, glm::vec2(0.0)));
    for (size_t i = 0; i < this->data.size(); i++) {
        for (size_t j = 0; j < this->data[i].size(); j++) {
            file >> this->data[i][j].x >> this->data[i][j].y;

            if (i == 0 && j == 0) {
                this->min_vector_magnitude = glm::length(this->data[i][j]);
                this->max_vector_magnitude = glm::length(this->data[i][j]);
            }

            this->min_vector_magnitude = min(this->min_vector_magnitude, glm::length(this->data[i][j]));
            this->max_vector_magnitude = max(this->max_vector_magnitude, glm::length(this->data[i][j]));
        }
    }

    file.close();

    this->min_vector_magnitude *= this->min_vector_magnitude;
    this->max_vector_magnitude *= this->max_vector_magnitude;

    vector<GLfloat> border{
        0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, (float)y, 1.0f, 1.0f, 1.0f,

        0.0f, (float)y, 1.0f, 1.0f, 1.0f,
        (float)x, (float)y, 1.0f, 1.0f, 1.0f,

        (float)x, (float)y, 1.0f, 1.0f, 1.0f,
        (float)x, 0.0f, 1.0f, 1.0f, 1.0f,

        (float)x, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 1.0f
    };

    this->_vertex.insert(this->_vertex.end(), border.begin(), border.end());
}

bool StreamLine::check(glm::vec2 position)
{
    return (
        position.x >= 0.0 &&
        position.y >= 0.0 &&
        position.x < this->data.size() &&
        position.y < this->data.size()
    );
}

glm::vec2 StreamLine::vector_interpolation(glm::vec2 position)
{
    if (this->check(position) == false) return glm::vec2(0.0);

    size_t x = position.x, y = position.y;

    glm::vec2 v11 = this->data[x][y];
    glm::vec2 v21 = this->data[min(this->data.size() - 1, x + 1)][y];
    glm::vec2 v12 = this->data[x][min(this->data[0].size() - 1, y + 1)];
    glm::vec2 v22 = this->data[min(this->data.size() - 1, x + 1)][min(this->data[0].size() - 1, y + 1)];

    float mu_x, mu_y;

    mu_x = position.x - (int)position.x;
    mu_y = position.y - (int)position.y;

    glm::vec2 temp1 = (1.0f - mu_x) * v11 + mu_x * v21;
    glm::vec2 temp2 = (1.0f - mu_x) * v12 + mu_x * v22;

    glm::vec2 result = (1.0f - mu_y) * temp1 + mu_y * temp2;

    return result;
}

glm::vec2 StreamLine::rk2(glm::vec2 position, float h)
{
    glm::vec2 position_vector = this->vector_interpolation(position);
    if (glm::length(position_vector) < EPSILON) return position;

    glm::vec2 temp = position + h * position_vector;

    return position + h * (position_vector + this->vector_interpolation(temp)) / 2.0f;
}

vector<GLfloat> StreamLine::calculate(glm::vec2 position, float delta, vector<vector<bool>> &table, int scale)
{
    vector<GLfloat> result;

    glm::vec2 start = position;

    for (auto i = 0; i < 10000; i++) {
        glm::vec2 temp = this->rk2(position, delta);

        if ((int)temp.x != (int)start.x || (int)temp.y != (int)start.y) {
            if (this->check(temp) == false || table[(int)(temp.x * scale)][(int)(temp.y * scale)]) break;

            table[(int)(temp.x * scale)][(int)(temp.y * scale)] = true;

            start = temp;

            float temp_magnitude = glm::length(this->vector_interpolation(temp));
            int color_index = ((temp_magnitude - this->min_vector_magnitude) / this->max_vector_magnitude) * 255.0;

            glm::vec3 color;
            for (auto i = 0; i < 3; i++) {
                color[i] = min(1.0, max(0.0, (double)RGBTABLE[color_index][i] / 255.0));
            }

            for (auto j = 0; j < 2; j++) {
                result.push_back(temp.x);
                result.push_back(temp.y);

                result.push_back(color[0]);
                result.push_back(color[1]);
                result.push_back(color[2]);
            }
        }

        position = temp;
    }

    if (result.size() != 0) {
        result.erase(result.begin(), result.begin() + 5);
        result.erase(result.end() - 5, result.end());
    }

    return result;
}

void StreamLine::run()
{
    int max_scale = 2;

    vector<vector<bool>> table(this->data.size() * max_scale, vector<bool>(this->data[0].size() * max_scale, false));
    for (int scale = max_scale; scale >= 1; scale >>= 1) {
        cout << "scale: " << (max_scale / scale) << '\n';

        float stride = (float)scale / (float)max_scale;
        for (size_t i = 0, count_i = 0; i < table.size(); i += scale, count_i++) {
            for (size_t j = 0, count_j = 0; j < table[i].size(); j += scale, count_j++) {
                if (table[i][j] == false) {
                    glm::vec2 start((float)count_i * stride, (float)count_j * stride);

                    vector<GLfloat> front = this->calculate(start, 0.1, table, scale);
                    vector<GLfloat> back = this->calculate(start, -0.1, table, scale);

                    if (front.size() != 0) this->_vertex.insert(this->_vertex.end(), front.begin(), front.end());
                    if (back.size() != 0) this->_vertex.insert(this->_vertex.end(), back.begin(), back.end());
                }
            }
        }
    }

    cout << "vertex size: " << this->_vertex.size() << '\n';
}

glm::vec3 StreamLine::shape()
{
    return glm::vec3(this->data.size(), this->data[0].size(), 0.0);
}

vector<GLfloat>& StreamLine::vertex()
{
    return this->_vertex;
}

vector<int> StreamLine::attribute()
{
    return vector<int>{2, 3};
}

GLenum StreamLine::render_mode()
{
    return GL_LINES;
}