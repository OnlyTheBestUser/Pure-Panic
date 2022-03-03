#version 330 core

uniform vec3  painterPosition;
uniform float radius;
uniform float hardness;
uniform float strength;
uniform vec4  colour;


in Vertex {
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 fragColour;
float lerp(float a, float b, float t){
    return (1.0f - t) * a + b * t;
}

float mask(vec3 position, vec3 center, float radius, float hardness){
    float m = distance(center, position);
    return 1 - smoothstep(radius * hardness, radius, m);    
}

void main(void)	{
// This world possition is the position of verticies not fragments.
    float f = mask(IN.worldPos, painterPosition, radius, hardness);
    float edge = f * strength;

    fragColour.rgb = mix(colour.rgb, painterPosition, edge);
    
    fragColour.a = 1;

	//fragColour = vec4(0.01,0,0,1);
}