#version 330 core

//Inputs
in vec3 tc;

//Uniforms
uniform samplerCube uEnvMap;

//Final fragment color
out vec4 color;

vec4 GetSkyBoxColor()
{
	vec4 finalColor = texture(uEnvMap, tc);
	return finalColor;
}

void main()
{
	color = GetSkyBoxColor();
	//color = vec4(1.0,0.0,0.0,1.0);
}