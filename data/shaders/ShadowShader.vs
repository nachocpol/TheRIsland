#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 uModel;
uniform mat4 uLightSpaceMatrix;

void main()
{
	gl_Position = uLightSpaceMatrix * uModel * vec4(position,1.0);
}