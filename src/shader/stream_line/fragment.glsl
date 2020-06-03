#version 440 core
in float slice_check;

out vec4 fragment_color;

uniform vec4 object_color;

void main()
{
    if (slice_check < 0.0) discard;

    fragment_color = object_color;
}