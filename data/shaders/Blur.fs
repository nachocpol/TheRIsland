#version 330 core

in vec2 oUv;
uniform sampler2D uScreenTexture;
uniform bool uBlurVertical;

float blurKernel[5] = float[](0.383103,0.241843,0.060626,0.00598,0.000229);

out vec4 color;

vec4 GetColor()
{
	//4 samples left and 4 right = 8
	//4 samples up   and 4 down  = 8
	int samples = 4;
	vec4 finalColor = texture2D(uScreenTexture,oUv)*blurKernel[0];
	vec2 tSize = 1.0 / textureSize(uScreenTexture,0);

	//Perform vertical blur
	if(uBlurVertical == true)
	{
		for(int i=1;i<=samples;i++)
		{
			vec2 tc = oUv;
			tc += vec2(0.0,tSize.y*i);
			finalColor += texture2D(uScreenTexture,tc)*blurKernel[i];

			tc = oUv;
			tc -= vec2(0.0,tSize.y*i);
			finalColor += texture2D(uScreenTexture,tc)*blurKernel[i];
		}
	}
	//Perform horizontal blur
	else
	{
		for(int i=1;i<=samples;i++)
		{
			vec2 tc = oUv;
			tc += vec2(tSize.x*i,0.0);
			finalColor += texture2D(uScreenTexture,tc)*blurKernel[i];

			tc = oUv;
			tc -= vec2(tSize.x*i,0.0);
			finalColor += texture2D(uScreenTexture,tc)*blurKernel[i];
		}
	}
	return finalColor;
}
void main()
{
	color = GetColor();
}
