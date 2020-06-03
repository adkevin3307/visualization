#version 440 core
layout (location = 0) in vec2 pos;

uniform mat4 projection_view_model;

void main()
{
    gl_Position = projection_view_model * vec4(pos, 0.0, 1.0);
}