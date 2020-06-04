#version 440 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 color;

out vec3 object_color;

uniform mat4 projection_view_model;

void main()
{
    object_color = color;

    gl_Position = projection_view_model * vec4(pos, 0.0, 1.0);
}