#version 440 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 texture_pos;

out float slice_check;
out vec3 fragment_pos;
out vec3 fragment_texture_pos;

uniform vec4 clip_plane;
uniform mat4 model;
uniform mat4 projection_view_model;

void main()
{
    fragment_pos = vec3(model * vec4(pos, 1.0));
    fragment_texture_pos = texture_pos;

    slice_check = dot(vec4(fragment_pos, 1.0), clip_plane);

    gl_Position = projection_view_model * vec4(pos, 1.0);
}