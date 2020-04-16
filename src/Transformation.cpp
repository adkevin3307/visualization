#include "Transformation.h"

#include <glm/ext.hpp>

using namespace std;

Transformation::Transformation(Shader shader)
{
    this->_model = glm::mat4(1.0f);
    this->_view = glm::mat4(1.0f);
    this->_projection = glm::mat4(1.0f);

    this->shader = shader;
}

Transformation::~Transformation()
{

}

void Transformation::set_model(TRANSFORMATION type, glm::vec3 v, float angle)
{
    switch (type) {
    case TRANSFORMATION::TRANSLATE:
        this->_model = glm::translate(this->_model, v);
        break;
    case TRANSFORMATION::SCALE:
        this->_model = glm::scale(this->_model, v);
        break;
    case TRANSFORMATION::ROTATE:
        this->_model = glm::rotate(this->_model, angle, v);
        break;
    default:
        break;
    }
    this->shader.set_uniform("model", this->_model);
}

void Transformation::set_view(glm::mat4 view)
{
    this->_view = view;
}

void Transformation::set_projection(GLfloat width, GLfloat height, GLfloat rate, GLfloat near, GLfloat far)
{
    GLfloat left = -width / rate;
    GLfloat right = width / rate;
    GLfloat bottom = -height / rate;
    GLfloat top = height / rate;

    this->_projection = glm::ortho(left, right, bottom, top, near, far);
}

void Transformation::set()
{
    this->shader.set_uniform("projection_view_model", (this->_projection * this->_view * this->_model));
}