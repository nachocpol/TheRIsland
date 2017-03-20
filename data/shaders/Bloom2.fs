#version 330 core

in vec2 oUv;
uniform sampler2D uScreenTexture;
uniform sampler2D uStandarTexture;	

out vec4 color;

vec4 GetColor()
{
	vec4 finalColor;// = texture2D(uScreenTexture,oUv);
	finalColor += texture2D(uStandarTexture,oUv) - vec4(0.25);
	return finalColor;
}

void main()
{
	color = GetColor();
}
