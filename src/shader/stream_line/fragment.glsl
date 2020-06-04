#version 440 core
in vec3 object_color;

out vec4 fragment_color;

void main()
{
    fragment_color = vec4(object_color, 1.0);
}