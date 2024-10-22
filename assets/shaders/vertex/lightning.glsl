#version 330
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

out vec3 color;
out vec4 out_world_pos;
out vec3 out_world_normal;

void main() {
    color = in_normal;
    out_world_pos = modelMatrix * vec4(in_position, 1.0f);
    out_world_normal = normalMatrix * in_normal; //příště
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0);
};
