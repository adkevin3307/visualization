#version 440 core
in vec3 fragment_pos;
in vec3 fragment_texture_pos;
in float slice_check;

out vec4 fragment_color;

layout (binding = 0) uniform sampler1D texture_1d;
layout (binding = 1) uniform sampler3D texture_3d;
uniform vec3 view_pos;
uniform vec3 light_pos;
uniform vec3 light_color;

void main()
{
    if (slice_check < 0.0) discard;

    float value = texture(texture_3d, fragment_texture_pos).w;
    vec3 normal = normalize(texture(texture_3d, fragment_texture_pos).xyz);

    vec4 object_color = texture(texture_1d, value);

    vec3 view_direction = normalize(fragment_pos - view_pos);

    if (dot(normal, view_direction) < 0.0) normal = -normal;

    vec3 light_direction = normalize(fragment_pos - light_pos);
    vec3 reflect_direction = reflect(light_direction, normal);

    vec3 ambient = 0.01 * light_color;
    vec3 diffuse = max(dot(normal, light_direction), 0.0) * light_color;
    vec3 specular = 0.5 * pow(max(dot(view_direction, -reflect_direction), 0.0), 8) * light_color;

    vec4 coefficient = vec4(clamp((ambient + diffuse + specular), vec3(0.0), vec3(1.0)), 1.0);
    vec4 result = coefficient * object_color;
    fragment_color = result;
}