#version 330 core

//Default attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in mat4 instModelMatrix;

layout(std140)uniform uDrawData
{
	mat4 uView;
	mat4 uProjection;
};

//Uniforms
uniform float uTime;

//Shader constants
const float windSpeed = 2.5f;
const float windAmp = 0.1f;

//Out to the fragment
out DATA
{
	vec2 texCoord;
	vec3 norm;
	vec3 localPos;
	vec3 pos;
	vec3 tang;
	vec3 bitan;
}vs_out;

void main()
{
	float windZ = 0.0;
	if(position.y == 1.0)
	{
		windZ = sin(uTime*windSpeed)*windAmp;
	}

	vec4 pos = instModelMatrix * vec4(position,1.0);
	pos.z += windZ;

	vs_out.localPos = position;
	vs_out.pos = pos.xyz;
	vs_out.texCoord = texcoord;
	vs_out.norm = normal;

	gl_Position = uProjection * uView * pos;
}