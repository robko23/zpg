#version 430

layout(location = 0) in vec3 vp;
layout(location = 1) in vec3 vn;
layout(location = 2) in vec2 vt;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 vt_out;
out vec4 out_world_pos;
out vec3 out_world_normal;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);
    vt_out = vt;
    out_world_pos = modelMatrix * vec4(vp, 1.0f);
    mat3 normal = transpose(inverse(mat3(modelMatrix)));
    out_world_normal = normalize(normal * vn);
}
