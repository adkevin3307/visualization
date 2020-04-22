#include "Mesh.h"

#include <numeric>

using namespace std;

Mesh::Mesh()
{
    this->stride = 1;
    this->attribute.resize(1, 0);
    this->render_mode = GL_POINTS;
    this->shape = glm::vec3(0.0);

    this->vertex.clear();
    this->vertex.shrink_to_fit();
}

Mesh::Mesh(Method *render_method, METHOD method) : Mesh::Mesh()
{
    this->vertex = render_method->vertex();
    this->attribute = render_method->attribute();
    this->render_mode = render_method->render_mode();
    this->shape = render_method->volume_shape();

    this->_method = method;

    this->stride = accumulate(this->attribute.begin(), this->attribute.end(), 0);

    if (this->stride == 0) this->stride = 1;
}

Mesh::~Mesh()
{

}

METHOD Mesh::method()
{
    return this->_method;
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

void Mesh::draw(GLenum rasterize_mode)
{
    BufferManagement::bind(this->buffer);
    BufferManagement::draw(this->buffer, 0, (this->vertex.size() / this->stride), this->render_mode, rasterize_mode);
    BufferManagement::unbind();
}