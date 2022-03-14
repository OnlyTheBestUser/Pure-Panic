#version 330 core

uniform mat4  modelMatrix;

uniform vec3  barycentricCoord;
uniform vec3  collisionPoint;
uniform vec2  nearTexCoord_a;
uniform vec2  nearTexCoord_b;
uniform vec2  nearTexCoord_c;
uniform vec2 uvHitPoint;

uniform vec2 viewport;

uniform vec2  textureSize;
uniform vec3 textureScale;
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

float mask(vec2 _position, vec2 _center, float _radius, float _hardness){
    float m = distance(_center, _position);
    return 1 - smoothstep(_radius * _hardness, _radius, m);    
}

void main(void)	{
// This world possition is the position of verticies not fragments.

    float avgScale = (textureScale.x + textureScale.y + textureScale.z) / 3;

    float f = mask(gl_FragCoord.xy / viewport,uvHitPoint, radius/avgScale, hardness);

    float edge = f * strength;

    fragColour.rgba = mix(fragColour.rgba, vec4(colour.rgb, 1), edge);
    //fragColour.r = fragColour.a;
    //fragColour.g = fragColour.a;
    //fragColour.b = fragColour.a;

    //fragColour.a = 1;
}