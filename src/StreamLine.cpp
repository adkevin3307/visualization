#include "StreamLine.h"

#include "constant.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <io.h>

using namespace std;

StreamLine::StreamLine(string filename)
{
    if (access(filename.c_str(), F_OK) == -1) throw runtime_error(filename + " Not Exist");

    this->load_data(filename);
}

StreamLine::~StreamLine()
{

}

void StreamLine::load_data(string filename)
{
    fstream file;
    file.open(filename, ios::in);

    int x, y;
    file >> x >> y;

    cout << "size: " << x << ", " << y << '\n';

    this->data.resize(x + 1, vector<glm::vec2>(y + 1, glm::vec2(0.0)));
    for (auto i = 0; i < x; i++) {
        for (auto j = 0; j < y; j++) {
            file >> this->data[i][j].x >> this->data[i][j].y;

            if (glm::length(this->data[i][j]) > EPSILON) this->data[i][j] = glm::normalize(this->data[i][j]);
        }
    }

    file.close();

    vector<GLfloat> border{
        0.0f, 0.0f,
        0.0f, (float)y,

        0.0f, (float)y,
        (float)x, (float)y,

        (float)x, (float)y,
        (float)x, 0.0f,

        (float)x, 0.0f,
        0.0f, 0.0f
    };

    this->_vertex.insert(this->_vertex.end(), border.begin(), border.end());
}

bool StreamLine::check(glm::vec2 position)
{
    return (
        position.x >= 0.0 &&
        position.y >= 0.0 &&
        position.x < this->data.size() - 1 &&
        position.y < this->data.size() - 1
    );
}

glm::vec2 StreamLine::vector_interpolation(glm::vec2 position)
{
    if (this->check(position) == false) return glm::vec2(0.0);

    glm::vec2 v11 = this->data[(int)position.x][(int)position.y];
    glm::vec2 v21 = this->data[(int)position.x + 1][(int)position.y];
    glm::vec2 v12 = this->data[(int)position.x][(int)position.y + 1];
    glm::vec2 v22 = this->data[(int)position.x + 1][(int)position.y + 1];

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

vector<GLfloat> StreamLine::calculate(glm::vec2 position, float delta, vector<vector<bool>> &table)
{
    vector<GLfloat> result;

    for (auto i = 0; i < 1000000; i++) {
        glm::vec2 temp = this->rk2(position, delta);

        if (this->check(temp) == false || table[(int)temp.x][(int)temp.y]) break;

        table[(int)temp.x][(int)temp.y] = true;

        for (auto j = 0; j < 2; j++) {
            result.push_back(temp.x);
            result.push_back(temp.y);
        }

        position = temp;
    }

    if (result.size() != 0) {
        result.erase(result.begin());
        result.erase(result.begin());
        result.pop_back();
        result.pop_back();
    }

    return result;
}

void StreamLine::run()
{
    int max_scale = 4;

    for (int scale = 1; scale <= max_scale; scale <<= 1) {
        vector<vector<bool>> table(this->data.size() * scale, vector<bool>(this->data[0].size() * scale, false));

        cout << "scale: " << scale << '\n';
        for (size_t i = 0; i < this->data.size() * scale; i++) {
            for (size_t j = 0; j < this->data[(int)(i / scale)].size() * scale; j++) {
                glm::vec2 start((float)i / (float)scale, (float)j / (float)scale);

                vector<GLfloat> front = this->calculate(start, 1.0, table);
                vector<GLfloat> back = this->calculate(start, -1.0, table);

                if (front.size() != 0) this->_vertex.insert(this->_vertex.end(), front.begin(), front.end());
                if (back.size() != 0) this->_vertex.insert(this->_vertex.end(), back.begin(), back.end());
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
    return vector<int>{2};
}

GLenum StreamLine::render_mode()
{
    return GL_LINES;
}