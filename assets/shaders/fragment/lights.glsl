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

// More info about memory layout here:
// https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout
// Keep in sync with ShderLights.h -> Light
struct Light {
    vec3 position; // treated as vec4 - 16 bytes
    vec3 direction; // treated as vec4 - 16 bytes
    vec3 attenuation; // treated as vec4 - 16 bytes
    vec4 color; // treated as vec4 - 16 bytes
    int type; // 4 bytes
    float cutoff; // 4 bytes
    // 8 bytes padding - aligned to 16 bytes
};

// Keep in sync with ShaderLights.h -> Material
struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform vec3 cameraPosition;

uniform Material material;

layout (std430, binding = 0) buffer Lights {
    Light lights[];
};

void main() {
    vec3 local_pos = out_world_pos.xyz / out_world_pos.w;

    bool has_ambient = (flags & (1 << 0)) != 0;
    bool has_diffuse = (flags & (1 << 1)) != 0;
    bool has_specular = (flags & (1 << 2)) != 0;
    bool has_halfway = (flags & (1 << 3)) != 0;

    frag_colour = vec4(0);
    if (has_ambient) {
        // Constant
        frag_colour = material.ambient;
    }

    for (int i = 0; i < lights.length(); i++) {
        vec3 lightVector = lights[i].position - local_pos;
        vec3 lightDir = normalize(lightVector);
        vec3 viewDir = normalize(cameraPosition - local_pos);
        float distance = length(lightVector);

        float attenuation = 1.0;
        if (lights[i].type == 1 || lights[i].type == 3) {
            float constant = lights[i].attenuation.x;
            float linear = lights[i].attenuation.y;
            float quadratic = lights[i].attenuation.z;
            attenuation = clamp(1.0 / (constant + linear * distance + quadratic * (distance * distance)), 0.0, 1.0);
        }

        if (has_diffuse) {
            // Lambert
            float diffuse_factor = max(dot(lightDir, out_world_normal), 0.0);
            vec4 diffuse_color = diffuse_factor * lights[i].color * material.diffuse;
            frag_colour += diffuse_color * attenuation;
        }

        if (has_specular) {
            if (dot(out_world_normal, lightDir) >= 0.0) {
                float specular_factor = 0.0;
                if (has_halfway) {
                    // Blinn phong
                    vec3 halfVector = normalize(lightDir + viewDir);
                    specular_factor = pow(max(dot(out_world_normal, halfVector), 0.0), material.shininess);
                } else {
                    // Phong
                    vec3 reflect_dir = reflect(-lightDir, out_world_normal);
                    specular_factor = pow(max(dot(viewDir, reflect_dir), 0.0), material.shininess);
                }

                vec4 specular_color = specular_factor * lights[i].color * material.specular * attenuation;
                frag_colour += specular_color;
            }
        }
    }
}