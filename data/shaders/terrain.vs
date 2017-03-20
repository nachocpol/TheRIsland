#version 330 core
#define MAX_LIGHTS 5

//Default attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

//Default uniforms
uniform mat4 uModel;

layout(std140)uniform DrawData
{
	mat4 uView;
	mat4 uProjection;
};

layout(std140)uniform ClipPlanes
{
	vec4 cplane0;
};

//Output to the fragment
out DATA 
{
	vec2 texCoord;
	vec3 pos;
	vec3 lPos;
	vec3 normal;
	mat3 TBN;
	vec4 posLightSpace;
}vs_out;

void main()
{
	vec4 wPos = vec4(uModel * vec4(position,1.0));

	vs_out.texCoord = texcoord;
	vs_out.pos = wPos.xyz;
	vec3 norm = normalize(vec3(uModel*vec4(normal,0.0)));
	vec3 tang = normalize(vec3(uModel*vec4(tangent,0.0)));
	vec3 bita = normalize(cross(tang,norm));
	vs_out.normal = norm;
	vs_out.TBN = mat3(tang,bita,norm);

	gl_Position = uProjection * uView * wPos;
	gl_ClipDistance[0] = dot(wPos,cplane0);
}