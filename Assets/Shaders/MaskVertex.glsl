#version 330 core

uniform  mat4  modelMatrix;

in   vec3  position;
in   vec2  texCoord;

out  Vertex   {
	vec2 texCoord;
	vec3 worldPos;
} OUT;

void  main(void){
	gl_Position = vec4(position , 1.0);
	OUT.worldPos = (modelMatrix * vec4(position , 1.0)).xyz;
	OUT.texCoord = (vec4(texCoord , 0.0,  1.0)). xy;
}