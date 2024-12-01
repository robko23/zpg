#version 430

in vec2 vt_out;

uniform sampler2D textureUnitId;

out vec4 frag_colour;

void main() {
    frag_colour = texture(textureUnitId, vt_out);
	// frag_colour = vec4(1);
}
