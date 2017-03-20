#version 330 core

in vec2 oUv;
uniform sampler2D uScreenTexture;

out vec4 color;

vec4 GetColor()
{
	vec4 finalColor = vec4(0.0);
	vec4 tCol = texture2D(uScreenTexture,oUv);
	float brig = dot(tCol.rgb, vec3(0.2126, 0.7152, 0.0722));
	//Output if the color is bright
	if(brig > 0.95)
	{
		finalColor = tCol;
		return finalColor;
	}
	//If not just output black
	return finalColor;
}

void main()
{
	color = GetColor();
}
