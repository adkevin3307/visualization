#include "StreamLine.h"

#include "constant.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <unistd.h>

using namespace std;

StreamLine::StreamLine(string filename)
    : _max_scale(-1), min_vector_magnitude(0.0), max_vector_magnitude(0.0)
{
    if (access(filename.c_str(), F_OK) == -1) throw runtime_error(filename + " Not Exist");

    this->load_data(filename);

    this->tables.resize(2);
    this->_base_scale = 16;

    int size = this->_base_scale;
    for (size_t i = 0; i < this->tables.size(); i++, size <<= 1) {
        this->tables[i].resize(size, vector<bool>(size, false));
    }
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

    this->data.resize(x, vector<glm::vec2>(y, glm::vec2(0.0)));
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
}

bool StreamLine::inside(glm::vec2 position)
{
    return (
        position.x >= 0.0 &&
        position.y >= 0.0 &&
        position.x < this->data.size() &&
        position.y < this->data[0].size()
    );
}

glm::vec2 StreamLine::vector_interpolation(glm::vec2 position)
{
    if (this->inside(position) == false) return glm::vec2(0.0);

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

void StreamLine::push(glm::vec2 position, vector<GLfloat> &result)
{
    float magnitude = glm::length(this->vector_interpolation(position));
    int color_index = ((magnitude - this->min_vector_magnitude) / this->max_vector_magnitude) * 255.0;
    color_index = min(255, max(0, color_index));

    glm::vec3 color;
    for (auto i = 0; i < 3; i++) {
        color[i] = (double)RGBTABLE[color_index][i] / 255.0;
    }

    result.push_back(position.x);
    result.push_back(position.y);

    result.push_back(color[0]);
    result.push_back(color[1]);
    result.push_back(color[2]);
    result.push_back(1.0);

    result.push_back(-1.0);
}

glm::vec2 StreamLine::rk2(glm::vec2 position, float h)
{
    glm::vec2 position_vector = this->vector_interpolation(position);
    if (glm::length(position_vector) < EPSILON) return position;

    glm::vec2 temp = position + h * position_vector;

    return position + h * (position_vector + this->vector_interpolation(temp)) / 2.0f;
}

glm::ivec2 StreamLine::data2table(glm::vec2 position, int scale)
{
    glm::ivec2 result;

    result.x = (position.x / (float)this->data.size()) * this->tables[scale].size();
    result.y = (position.y / (float)this->data[0].size()) * this->tables[scale][0].size();

    return result;
}

vector<GLfloat> StreamLine::calculate(glm::vec2 position, float delta, int scale)
{
    vector<GLfloat> result;
    glm::vec2 start = position;

    this->push(position, result);

    for (auto i = 0; i < 10000; i++) {
        glm::vec2 temp = this->rk2(position, delta);

        glm::ivec2 start_index = this->data2table(start, scale);
        glm::ivec2 temp_index = this->data2table(temp, scale);

        if ((temp_index.x != start_index.x) || (temp_index.y != start_index.y)) {
            if (this->inside(temp) == false || this->tables[scale][temp_index.x][temp_index.y]) break;

            start = temp;
        }

        this->push(temp, result);

        for (size_t table_index = scale; table_index < this->tables.size(); table_index++) {
            glm::ivec2 position_index = this->data2table(temp, table_index);

            this->tables[table_index][position_index.x][position_index.y] = true;
        }

        position = temp;
    }

    if (result.size() != 0 && delta < 0) {
        int count = 0;
        for (size_t i = 0; i < result.size() / 2; i += 7) {
            for (auto delta = 0; delta < 7; delta++) {
                GLfloat temp;
                int back_index = result.size() - (count + 1) * 7;

                temp = result[i + delta];
                result[i + delta] = result[back_index + delta];
                result[back_index + delta] = temp;
            }
            count += 1;
        }
    }

    return result;
}

void StreamLine::run()
{
    for (size_t scale = 0; scale < this->tables.size(); scale++) {
        if (this->tables[scale].size() > this->data.size() || this->tables[scale][0].size() > this->data[0].size()) {
            this->_max_scale = scale - 1;
            break;
        }

        cout << "scale: " << (scale + 1) << '\n';

        for (size_t i = 0; i < this->tables[scale].size(); i++) {
            for (size_t j = 0; j < this->tables[scale][i].size(); j++) {
                if (this->tables[scale][i][j] == false) {
                    glm::vec2 start;
                    start.x = (((float)i + 0.5) / (float)this->tables[scale].size()) * (float)this->data.size();
                    start.y = (((float)j + 0.5) / (float)this->tables[scale][i].size()) * (float)this->data[i].size();

                    vector<GLfloat> front_line = this->calculate(start, 0.1, scale);
                    vector<GLfloat> back_line = this->calculate(start, -0.1, scale);

                    vector<GLfloat> line;

                    if (back_line.size() > 0) line.insert(line.end(), back_line.begin(), back_line.end());
                    if (front_line.size() > 0) line.insert(line.end(), front_line.begin(), front_line.end());

                    if (line.size() > 0) {
                        int count = 0;
                        double max_size = 3.5, min_size = 1.0;
                        double range = (max_size - min_size) / (line.size() / 7 - 1);

                        for (size_t line_index = 6; line_index < line.size(); line_index += 7, count++) {
                            line[line_index] = max(min_size, max_size - range * count);
                        }

                        this->_vertex.insert(this->_vertex.end(), line.begin(), line.end());
                    }
                }
            }
        }
    }

    if (this->_max_scale == -1) {
        this->_max_scale = this->tables.size() - 1;
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
    return vector<int>{2, 4, 1};
}

GLenum StreamLine::render_mode()
{
    return GL_POINTS;
}

int StreamLine::base_scale()
{
    return this->_base_scale;
}

int StreamLine::max_scale()
{
    return this->_max_scale;
}
