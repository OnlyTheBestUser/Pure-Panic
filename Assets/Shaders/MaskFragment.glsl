#version 330 core

uniform sampler2D maskTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)	{
	vec4 samp = texture(maskTex,IN.texCoord);
	fragColour = samp;
}