#version 430 core
out vec4 frag_colour;

in vec4 out_world_pos;
in vec3 out_world_normal;


struct PhongLight {
    vec3 position; // treated as vec4 - 16 bytes
    vec4 diffuse;
    float intensity;
};

layout (std430, binding = 0) buffer Lights {
    PhongLight lights[];
};

void main() {
    vec3 local_pos = out_world_pos.xyz / out_world_pos.w;
    vec3 normal_normalized = normalize(out_world_normal);

    vec4 final_color = vec4(0);

    for (int i = 0; i < lights.length(); i++) {
        vec3 lightVector = lights[i].position - local_pos;
        float dp = max(dot(lightVector, normal_normalized), 0.0);
        vec4 final_light = dp * lights[i].diffuse;

        final_color += final_light * lights[i].intensity;
    }
    frag_colour = ambient + final_color;
}