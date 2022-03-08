#version 330 core

uniform  mat4  modelMatrix;
uniform vec3  painterPosition;
uniform vec2 nearTexCoord;
uniform vec2 textureSize;
uniform float textureScale;
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

float mask(vec3 _position, vec3 _center, float _radius, float _hardness){
    float m = distance(_center, _position);
    return 1 - smoothstep(_radius * _hardness, _radius, m);    
}

void main(void)	{
// This world possition is the position of verticies not fragments.

    vec2 fragCoordScaled = (gl_FragCoord.xy/textureSize.x);
    vec3 fragWorldPos = (modelMatrix * vec4(-fragCoordScaled.x, painterPosition.y , -fragCoordScaled.y, 1.0)).xyz;
    //float f = mask(fragWorldPos, (painterPosition-(vec3(textureSize.x, 0, textureSize.y)/2)) - vec3(radius, 0 , radius), radius, hardness);
    float f = mask(fragWorldPos + vec3(textureSize.x,0,textureSize.y)/1.6, painterPosition, radius, hardness);

    float edge = f * strength;

    fragColour.rgb = mix(fragColour.rgb, colour.rgb, edge);

    fragColour.a = 1;

	//fragColour = vec4(0.01,0,0,1);
}