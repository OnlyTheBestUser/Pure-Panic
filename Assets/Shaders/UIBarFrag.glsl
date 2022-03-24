#version 420 core

uniform vec2 ratio;
uniform vec4 team1Colour;
uniform vec4 team2Colour;
uniform vec2 screenSize;

in Vertex
{
	vec4 colour;
} IN;

out vec4 fragColor;

void main(void)
{

	vec2 curScreenCoord = gl_FragCoord.xy / (screenSize/2 + screenSize/2); 
	if(curScreenCoord.x < ratio.x){
		fragColor = team1Colour;
	} else {
		fragColor = team2Colour;
	}

}