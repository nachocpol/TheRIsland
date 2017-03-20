#version 430 core

#define MAX_LIGHTS 5

struct Light
{
	vec3 lColor;
	vec3 lDir;
	mat4 lMatrix;
};

//Default attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

//Default uniforms
uniform mat4 uModel;
uniform Light[MAX_LIGHTS] uLights;
uniform float uCurLights;

layout(std140)uniform uDrawData
{
	mat4 uView;
	mat4 uProjection;
};

//Out to the fragment
out DATA
{
	vec2 texCoord;
	vec3 norm;
	vec3 pos;
	vec3 tang;
	vec3 bitan;
	vec3 posLightSpace;
}vs_out;


void main()
{
	vec4 pos = uModel * vec4(position,1.0);
	vs_out.pos = pos.xyz;
	gl_Position = uProjection * uView * pos;

	vs_out.texCoord = texcoord;

	mat3 model = mat3(uModel);

	vs_out.norm = normalize(model * normal);
	vs_out.tang = normalize(model * tangent);
	vs_out.bitan = normalize(model * cross(vs_out.norm,vs_out.tang));
}