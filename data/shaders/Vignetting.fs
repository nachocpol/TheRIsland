#version 330 core

in vec2 oUv;
uniform sampler2D uScreenTexture;
uniform vec2 uScreenResolution;

out vec4 color;

const float strenght = 1.1;

vec4 CalcVignetting()
{
	vec4 finalColor = texture(uScreenTexture,oUv);

	//Vector from this pos to the center
    vec2 toCenter = (gl_FragCoord.xy / uScreenResolution.xy) - vec2(0.5);
    float len = 1.0 - length(toCenter * strenght); //Inverse it (black outside)

    //return vec4(len);
	return vec4(finalColor * (len * strenght));
}

void main()
{ 
	color = CalcVignetting();
}
