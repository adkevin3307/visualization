#include "Mesh.h"

using namespace std;

Mesh::Mesh()
{
    this->vertex.clear();
    this->vertex.shrink_to_fit();
    this->shape = glm::vec3(0.0, 0.0, 0.0);
    this->stride = 0;
}

Mesh::Mesh(vector<GLfloat> &vertex, glm::vec3 shape, int stride) : Mesh::Mesh()
{
    this->vertex = vertex;
    this->shape = shape;
    this->stride = stride;
}

Mesh::~Mesh()
{

}

void Mesh::init()
{
    this->buffer = BufferManagement::generate();
    BufferManagement::bind(this->buffer);
    BufferManagement::fill(vertex);
    BufferManagement::set(0, 3, this->stride, 0);
    BufferManagement::set(1, 3, this->stride, 3 * sizeof(GLfloat));
    BufferManagement::unbind();
}

void Mesh::transform(Transformation &transformation)
{
    transformation.set_model(TRANSFORMATION::TRANSLATE, -1.0f * this->shape / 2.0f);
}

void Mesh::draw(GLenum mode)
{
    BufferManagement::bind(this->buffer);
    BufferManagement::draw(this->buffer, 0, this->vertex.size() / this->stride, mode);
    BufferManagement::unbind();
}