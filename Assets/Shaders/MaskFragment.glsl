#version 330 core

uniform sampler2D maskTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

float mask(vec3 position, vec3 center, float radius, float hardness){
    float m = distance(center, position);
    return 1 - smoothstep(radius * hardness, radius, m);    
}

void main(void)	{
	vec4 samp = texture(maskTex,IN.texCoord);
	fragColour = samp;
}