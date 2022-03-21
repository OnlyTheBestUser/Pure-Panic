#version 420 core

layout (binding = 0) uniform sampler2D 	mainTex;
uniform int useTexture;

in Vertex
{
	vec4 colour;
} IN;

out vec4 fragColor;

void main(void)
{
	fragColor = IN.colour;
}