#version 430 core
out vec4 frag_colour;

in vec4 out_world_pos;
in vec3 out_world_normal;

// Bit masks
// LSB -> MSB
// first 3 bits - lighting
// 0 - has ambient
// 1 - has diffuse
// 2 - has specular
// 3 - use blinn phong instead of phong (bit 3 needs to be enabled)
uniform int flags;

struct Light {
    vec3 position; // treated as vec4 - 16 bytes
    // More info about memory layout here:
    // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout
    vec4 color;
    float intensity;
};

uniform vec4 ambientColor;

uniform vec3 cameraPosition;

layout (std430, binding = 0) buffer Lights {
    Light lights[];
};

void main() {
    vec3 local_pos = out_world_pos.xyz / out_world_pos.w;
    vec3 normal_normalized = normalize(out_world_normal);

    bool has_ambient = (flags & (1 << 0)) != 0;
    bool has_diffuse = (flags & (1 << 1)) != 0;
    bool has_specular = (flags & (1 << 2)) != 0;
    bool has_halfway = (flags & (1 << 3)) != 0;

    frag_colour = vec4(0);
    if (has_ambient) {
        // Constant
        frag_colour = ambientColor + frag_colour;
    }

    for (int i = 0; i < lights.length(); i++) {
        vec3 lightVector = lights[i].position - local_pos;
        vec3 lightDir = normalize(lightVector);
        vec3 viewDir = normalize(cameraPosition - local_pos);
        if (has_diffuse) {
            // Lambert
            float diffuse_grey = max(dot(lightVector, normal_normalized), 0.0);
            vec4 diffuse_color = diffuse_grey * lights[i].color;
            vec4 diffuse_intensity = diffuse_color * lights[i].intensity;

            frag_colour += diffuse_intensity;
        }

        if (has_specular) {
            float spec;
            float specular_strength = 0.5;
            if (has_halfway) {
                // Blinn phong
                vec3 halfwayDir = normalize(lightDir - viewDir);
                spec = pow(max(dot(normal_normalized, halfwayDir), 0.0), 64);
            } else {
                // Phong
                vec3 reflect_dir = reflect(-lightDir, normal_normalized);
                spec = pow(max(dot(viewDir, reflect_dir), 0.0), 32);
            }

            frag_colour += specular_strength * spec * vec4(1);
        }
    }
}