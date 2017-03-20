#version 330 core

uniform sampler2D uScreenTexture;
uniform sampler2D uStandarTexture;
uniform vec2 uScreenResolution;

in vec2 oUv;

out vec4 color;

vec4 GetColor()
{
  vec4 finalColor = vec4(0.0); 
  finalColor = texture2D(uStandarTexture,oUv);
  finalColor += texture2D(uScreenTexture,oUv);
  return finalColor;
}

void main()
{
	color = GetColor();
}

