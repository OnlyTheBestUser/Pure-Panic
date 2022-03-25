#version 420 core

uniform vec2 ratio;
uniform vec4 team1Colour;
uniform vec4 team2Colour;

in Vertex
{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColor;

void main(void)
{

	if(IN.texCoord.x < ratio.x){
		fragColor = team1Colour;
	} else {
		fragColor = team2Colour;
	}

}