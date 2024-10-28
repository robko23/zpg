#version 430 core
out vec4 frag_colour;

in vec4 out_world_pos;
in vec3 out_world_normal;

// Bit masks
// LSB -> MSB
// first 3 bits - lighting
// 1 - has ambient
// 2 - has diffuse
// 4 - has specular
uniform int flags;

struct Light {
    vec3 position; // treated as vec4 - 16 bytes
    // More info about memory layout here:
    // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout
    vec4 color;
    float intensity;
};

uniform vec4 ambient;

layout (std430, binding = 0) buffer Lights {
    Light lights[];
};

void main() {
    vec3 local_pos = out_world_pos.xyz / out_world_pos.w;
    vec3 normal_normalized = normalize(out_world_normal);

    bool has_ambient = (flags & (1 << 0)) != 0;
    bool has_diffuse = (flags & (1 << 1)) != 0;

    frag_colour = vec4(0);
    if (has_ambient) {
        frag_colour = ambient + frag_colour;
    }

    for (int i = 0; i < lights.length(); i++) {
        vec3 lightVector = lights[i].position - local_pos;
        if (has_diffuse) {
            float diffuse_grey = max(dot(lightVector, normal_normalized), 0.0);
            vec4 diffuse_color = diffuse_grey * lights[i].color;
            vec4 diffuse_intensity = diffuse_color * lights[i].intensity;

            frag_colour += diffuse_intensity;
        }
    }
}