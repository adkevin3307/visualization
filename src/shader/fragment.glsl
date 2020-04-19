#version 440 core
in vec3 fragment_pos;
in vec3 fragment_normal;
in float slice_check;

out vec4 fragment_color;

uniform vec3 view_pos;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec4 object_color;

void main()
{
    if (slice_check < 0.0) discard;

    vec3 normal = normalize(fragment_normal);
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