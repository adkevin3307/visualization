#version 440 core
layout (location = 0) in vec2 pos;

out float slice_check;

uniform vec4 clip_plane;
uniform mat4 model;
uniform mat4 projection_view_model;

void main()
{
    vec3 fragment_pos = vec3(model * vec4(pos, 0.0, 1.0));

    slice_check = dot(vec4(fragment_pos, 1.0), clip_plane);

    gl_Position = projection_view_model * vec4(pos, 0.0, 1.0);
}