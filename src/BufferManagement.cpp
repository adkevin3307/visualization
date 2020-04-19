#include "BufferManagement.h"

#include <stdint.h>

using namespace std;

Buffer BufferManagement::generate()
{
    Buffer buffer;

    glGenVertexArrays(1, &(buffer.vertex_array));
    glGenBuffers(1, &(buffer.vertex_buffer));

    return buffer;
}

void BufferManagement::bind(Buffer &buffer)
{
    glBindVertexArray(buffer.vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.vertex_buffer);
}

void BufferManagement::fill(vector<GLfloat> &vertex)
{
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(GLfloat), vertex.data(), GL_STATIC_DRAW);
}

void BufferManagement::set(GLuint index, GLint size, GLint stride, int offset)
{
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(intptr_t)offset);
    glEnableVertexAttribArray(index);
}

void BufferManagement::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void BufferManagement::draw(Buffer &buffer, int first, int count, GLenum render_mode, GLenum rasterize_mode)
{
    glPolygonMode(GL_FRONT_AND_BACK, rasterize_mode);
    glDrawArrays(render_mode, first, count);
}