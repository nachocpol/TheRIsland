#version 330 core

//Default attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;

//Default uniforms
uniform mat4 uModel;

layout(std140)uniform DrawData
{
	mat4 uView;
	mat4 uProjection;
};

uniform float uTime;

//Output to the fragment
out vec2 tc;
out vec3 norm;
out vec3 pos;
out vec4 clipSpace;

const float tiling = 24.0;

void main()
{
	//vec3 newPos = position;
	//newPos.y = sin(uTime*0.5)*0.1;

	clipSpace = uProjection * uView * uModel * vec4(position,1.0);
	gl_Position =  clipSpace;
	pos = (uModel * vec4(position,1.0)).xyz;

	//Normal with uniform transformations
	vec4 transNorm = transpose(inverse(uModel))*(vec4(normal,1.0));
	norm = transNorm.xyz;
	tc = texcoord * tiling;
}