#version 430 core
out vec4 frag_colour;

in vec4 out_world_pos;
in vec3 out_world_normal;
in vec2 vt_out;

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
    uint id; // 4 bytes
    // 4 bytes padding - aligned to 16 bytes
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

layout(std430, binding = 0) buffer Lights {
    Light lights[];
};

uniform sampler2D textureUnitId;

void main() {
    vec3 local_pos = out_world_pos.xyz / out_world_pos.w;

    frag_colour = texture(textureUnitId, vt_out);

    for (int i = 0; i < lights.length(); i++) {
        vec3 lightVector = vec3(1);
        float distance = 0;
        vec3 viewDir = normalize(cameraPosition - local_pos);
        float intensity = 1;

        if (lights[i].type == 1) { // Point light
            lightVector = lights[i].position - local_pos;
            distance = length(lightVector);
            lightVector = normalize(lightVector);
        } else if (lights[i].type == 2) { // Directional light
            lightVector = normalize(-lights[i].direction);
        } else if (lights[i].type == 3) { // Spotlight / reflector
            lightVector = lights[i].position - local_pos;
            distance = length(lightVector);
            lightVector = normalize(lightVector);

            float theta = dot(lightVector, normalize(-lights[i].direction));
            float epsilon = 0.07; // Controls the softness of the spotlight edge
            float innerCutoff = lights[i].cutoff; // Cosine of inner cutoff angle
            float outerCutoff = innerCutoff - epsilon; // Cosine of outer cutoff angle
            intensity = smoothstep(outerCutoff, innerCutoff, theta);

            if (intensity <= 0.0) continue; // Skip light if completely out of range
        } else {
            continue; // unsupported light
        }

        float attenuation = 1.0;
        if (lights[i].type == 1 || lights[i].type == 3) {
            float constant = lights[i].attenuation.x;
            float linear = lights[i].attenuation.y;
            float quadratic = lights[i].attenuation.z;
            attenuation = clamp(1.0 / (constant + linear * distance + quadratic * (distance * distance)), 0.0, 1.0);
        }

        // Lambert
        float diffuse_factor = max(dot(lightVector, out_world_normal), 0.0);
        vec4 diffuse_color = diffuse_factor * lights[i].color * material.diffuse;
        frag_colour += diffuse_color * attenuation * intensity;

        if (dot(out_world_normal, lightVector) >= 0.0) {
            float specular_factor = 0.0;
            // Blinn phong
            vec3 halfVector = normalize(lightVector + viewDir);
            specular_factor = pow(max(dot(out_world_normal, halfVector), 0.0), material.shininess);

            vec4 specular_color = specular_factor * lights[i].color * material.specular * attenuation;
            frag_colour += specular_color * intensity;
        }
    }
}
