#version 330 core

uniform sampler2D uScreenTexture;
uniform vec4 uSunPosition;
uniform vec2 uScreenResolution;

in vec2 oUv;

out vec4 color;

bool Vec3Compare(vec3 a,vec3 b)
{
	if( a.x == b.x &&
		a.y == b.y &&
		a.z == b.z)
	{
		return true;
	}
	return false;
}

vec4 GetSun()
{
	vec4 finalColor = vec4(0.0);
	vec4 viewport = vec4(0.0,0.0,uScreenResolution.xy);

	//Find our frag pos in normalized device space
	vec4 ndcPos;
	ndcPos.xy = ((2.0 * gl_FragCoord.xy) - (2.0 * viewport.xy)) / (viewport.zw) - 1;
	ndcPos.w = 1.0;

	//If the distance to the sun is small
	//output white (the sun is already in nds)
	if(distance(ndcPos.xy,uSunPosition.xy) < 0.05)
	{
		finalColor = vec4(1.0,0.87,0.6,1.0);
	}
	return finalColor;
}

vec4 GetColor()
{
	vec4 finalColor;
	vec3 curColor = texture(uScreenTexture,oUv).xyz;

	if(Vec3Compare(curColor,vec3(1.0)))
	{
		finalColor = vec4(0.0);
		finalColor += GetSun();
	}
	else
	{
		finalColor = vec4(0.0);
	}
	return finalColor;
}

void main()
{
	color = GetColor();
}

