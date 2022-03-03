#version 330 core

in Vertex {
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 fragColour;

float mask(vec3 position, vec3 center, float radius, float hardness){
    float m = distance(center, position);
    return 1 - smoothstep(radius * hardness, radius, m);    
}

void main(void)	{
	fragColour = vec4(1,0,0,1);
}