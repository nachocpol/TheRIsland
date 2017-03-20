#version 330 core

in vec2 oUv;
uniform sampler2D uScreenTexture;

out vec4 color;

void main()
{
	vec4 texColor = texture(uScreenTexture,oUv);
	color = texColor;
}
