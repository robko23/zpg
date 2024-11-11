#version 430 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec4 out_world_pos;
out vec3 out_world_normal;

void main() {
    out_world_pos = modelMatrix * vec4(in_position, 1.0f);
    mat3 normal = transpose(inverse(mat3(modelMatrix)));
    out_world_normal = normalize(normal * in_normal);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0);
};
