#version 330
out vec4 frag_colour;

uniform vec4 lightColor;

void main() {
    frag_colour = lightColor;
}
