#include "Mesh.h"

#include <numeric>

using namespace std;

Mesh::Mesh()
{
    this->stride = 0;
    this->attribute_size.resize(1, 0);
    this->shape = glm::vec3(0.0, 0.0, 0.0);

    this->vertex.clear();
    this->vertex.shrink_to_fit();
}

Mesh::Mesh(vector<GLfloat> &vertex, vector<int> attribute_size, glm::vec3 shape) : Mesh::Mesh()
{
    this->vertex = vertex;
    this->attribute_size = attribute_size;
    this->shape = shape;
    this->stride = accumulate(this->attribute_size.begin(), this->attribute_size.end(), 0);
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
    for (size_t i = 0; i < this->attribute_size.size(); i++) {
        BufferManagement::set(i, this->attribute_size[i], this->stride, sum * sizeof(GLfloat));
        sum += this->attribute_size[i];
    }

    BufferManagement::unbind();
}

void Mesh::transform(Transformation &transformation)
{
    transformation.set_model(TRANSFORMATION::TRANSLATE, -1.0f * this->shape / 2.0f);
}

void Mesh::draw(GLenum mode)
{
    BufferManagement::bind(this->buffer);
    BufferManagement::draw(this->buffer, 0, ((this->stride == 0) ? 0 : (this->vertex.size() / this->stride)), mode);
    BufferManagement::unbind();
}