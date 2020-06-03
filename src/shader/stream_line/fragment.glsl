#version 440 core
out vec4 fragment_color;

uniform vec4 object_color;

void main()
{
    fragment_color = object_color;
}