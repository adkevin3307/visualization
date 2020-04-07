#version 440 core
in vec3 fragment_pos;
in vec3 fragment_normal;

out vec4 fragment_color;

uniform vec3 view_pos;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 object_color;

void main()
{
    vec3 normal = normalize(fragment_normal);
    vec3 light_direction = normalize(light_pos - fragment_pos);
    vec3 view_direction = normalize(view_pos - fragment_pos);
    vec3 reflect_direction = reflect(-light_direction, normal);

    vec3 ambient = 0.01 * light_color;
    vec3 diffuse = max(dot(normal, light_direction), dot(-normal, light_direction)) * light_color;
    vec3 specular = 0.5 * pow(max(dot(view_direction, reflect_direction), 0.0), 32) * light_color;

    vec3 result = clamp((ambient + diffuse + specular), vec3(0.0), vec3(1.0)) * object_color;
    fragment_color = vec4(result, 1.0);
}