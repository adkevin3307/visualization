#version 440 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 texture_pos;

out vec3 fragment_pos;
out vec3 fragment_texture_pos;

uniform mat4 model;
uniform mat4 projection_view_model;

void main()
{
    fragment_pos = vec3(model * vec4(pos, 1.0));
    fragment_texture_pos = texture_pos;

    gl_Position = projection_view_model * vec4(pos, 1.0);
}