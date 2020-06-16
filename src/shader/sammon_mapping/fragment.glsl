#version 440 core
in vec4 object_color;

out vec4 fragment_color;

void main()
{
    fragment_color = object_color;
}