#include "Mesh.h"

#include <numeric>
#include <iostream>

using namespace std;

Mesh::Mesh()
{
    this->stride = 1;
    this->attribute.resize(1, 0);
    this->render_mode = GL_POINTS;
    this->shape = glm::vec3(0.0);

    this->_color = glm::vec4(1.0);

    this->vertex.clear();
    this->vertex.shrink_to_fit();
}

Mesh::Mesh(Method *method, glm::vec4 color) : Mesh::Mesh()
{
    this->vertex = method->vertex();
    this->attribute = method->attribute();
    this->render_mode = method->render_mode();
    this->shape = method->volume_shape();
    this->stride = accumulate(this->attribute.begin(), this->attribute.end(), 0);

    this->_color = color;

    if (this->stride == 0) this->stride = 1;
}

Mesh::~Mesh()
{

}

void Mesh::init()
{
    this->buffer = BufferManagement::generate();
    BufferManagement::bind(this->buffer);
    BufferManagement::fill(vertex);

    int sum = 0;
    for (size_t i = 0; i < this->attribute.size(); i++) {
        BufferManagement::set(i, this->attribute[i], this->stride, sum * sizeof(GLfloat));
        sum += this->attribute[i];
    }

    BufferManagement::unbind();
}

void Mesh::transform(Transformation &transformation)
{
    transformation.init(TRANSFORMATION::MODEL);
    transformation.set_model(TRANSFORMATION::TRANSLATE, -1.0f * this->shape / 2.0f);
}

void Mesh::color(Shader &shader)
{
    shader.set_uniform("object_color", this->_color);
}

void Mesh::draw(GLenum rasterize_mode)
{
    BufferManagement::bind(this->buffer);
    BufferManagement::draw(this->buffer, 0, (this->vertex.size() / this->stride), this->render_mode, rasterize_mode);
    BufferManagement::unbind();
}