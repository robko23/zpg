#version 330
out vec4 frag_colour;

in vec3 color;
in vec4 out_world_pos;
in vec3 out_world_normal;

void main() {
    vec3 local_pos = out_world_pos.xyz / out_world_pos.w;

    vec3 lightPosition = vec3(0, 0, 0); //správně upravit
    vec3 lightVector = lightPosition - local_pos;
    float dot_product = max(dot(lightVector, normalize(out_world_normal)), 0.0);
    vec4 diffuse = dot_product * vec4(0.385, 0.647, 0.812, 1.0);
    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);
    frag_colour = ambient + diffuse;
}