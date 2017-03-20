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

//Output to the fragment
out vec3 tc;

void main()
{
	mat4 cmView = mat4(mat3(uView));
	gl_Position = uProjection * cmView * vec4(position,1.0);
	tc = position;
}