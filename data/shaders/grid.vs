#version 330 core

layout (location = 0) in vec3 position;

//Default uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//This is only for the grid
out vec3 pos;

void main()
{
	pos = position;
	gl_Position = projection * view * model * vec4(position,1.0);
}