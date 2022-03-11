#version 420 core

layout(std140, binding = 0) uniform cameraMatrix
{
    mat4 projMatrix;
	mat4 viewMatrix;
};

uniform mat4 modelMatrix;
uniform mat4 invProjMatrix;

in  vec3 position;

out Vertex {
	vec3 viewDir;
} OUT;

void main(void)		{
	vec3 pos = position;
	pos.xy	  *= vec2(invProjMatrix[0][0],invProjMatrix[1][1]);
	pos.z 	= -1.0f;

	OUT.viewDir		= transpose(mat3(viewMatrix)) * normalize(pos);
	gl_Position		= vec4(position, 1.0);
}
