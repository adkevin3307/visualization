#version 440 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec4 color;

out vec4 object_color;

uniform mat4 projection_view_model;

void main()
{
    object_color = color;

    gl_PointSize = 3.0;
    gl_Position = projection_view_model * vec4(pos, 0.0, 1.0);
}