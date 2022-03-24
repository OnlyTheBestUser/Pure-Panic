#version 420 core

layout (binding = 0) uniform sampler2D 	mainTex;
uniform int useTexture;
uniform vec2 ratio;

uniform vec2 screenSize;

in Vertex
{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColor;

void main(void)
{
	fragColor = IN.colour;
	fragColor *= texture(mainTex, IN.texCoord);
	if(fragColor.a == 0){
		discard;
	}

}