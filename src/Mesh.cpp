#include "Mesh.h"

#include <numeric>

using namespace std;

Mesh::Mesh()
{
    this->stride = 1;
    this->attribute.resize(1, 0);
    this->render_mode = GL_POINTS;
    this->shape = glm::vec3(0.0);
    this->use_texture = false;

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

void Mesh::enable_texture(int size)
{
    this->use_texture = true;
    this->texture.resize(size);
    
    for (auto i = 0; i < size; i++) {
        glGenTextures(1, &(this->texture[i].first));
    }
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

void Mesh::init_texture(GLenum target, int index)
{
    if (this->use_texture == false) return;

    this->texture[index].second = target;

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(target, this->texture[index].first);

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Mesh::set_texture(int index, vector<float> &texture_data, glm::ivec3 shape)
{
    if (this->use_texture == false) return;

    glActiveTexture(GL_TEXTURE0 + index);
    switch (this->texture[index].second) {
        case GL_TEXTURE_1D:
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, shape.x, 0, GL_RGBA, GL_FLOAT, texture_data.data());
            glGenerateMipmap(GL_TEXTURE_1D);
            break;
        case GL_TEXTURE_2D:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shape.x, shape.y, 0, GL_RGBA, GL_FLOAT, texture_data.data());
            glGenerateMipmap(GL_TEXTURE_2D);
            break;
        case GL_TEXTURE_3D:
            glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, shape.x, shape.y, shape.z, 0, GL_RGBA, GL_FLOAT, texture_data.data());
            glGenerateMipmap(GL_TEXTURE_3D);
            break;
        default:
            break;
    }

    glBindTexture(this->texture[index].second, 0);
}

void Mesh::transform(Transformation &transformation)
{
    transformation.init(TRANSFORMATION::MODEL);
    transformation.set_model(TRANSFORMATION::TRANSLATE, -1.0f * this->shape / 2.0f);
}

void Mesh::draw(GLenum rasterize_mode)
{
    BufferManagement::bind(this->buffer);
    for (size_t i = 0; this->use_texture && i < this->texture.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(this->texture[i].second, this->texture[i].first);
    }
    BufferManagement::draw(this->buffer, 0, (this->vertex.size() / this->stride), this->render_mode, rasterize_mode);
    for (size_t i = 0; this->use_texture && i < this->texture.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(this->texture[i].second, 0);
    }
    BufferManagement::unbind();
}