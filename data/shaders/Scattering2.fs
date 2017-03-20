#version 330 core

float uExposure = 0.578;
float uDecay = 0.95; //0-1
float uDensity = 2.0;
float uWeight = 0.97;
int uSamples = 190;

uniform sampler2D uScreenTexture;
uniform vec4 uSunPosition;
uniform vec2 uScreenResolution;

in vec2 oUv;

out vec4 color;

vec4 GetColor()
{
	//Sun pos in texture coordinates
	vec2 sunPos =  uSunPosition.xy * 0.5 + 0.5;
  //if(sunPos.x > 1.0){sunPos.x = 1.0;}
  //if(sunPos.x < 0.0){sunPos.x = 0.0;}
  //if(sunPos.y > 1.0){sunPos.y = 1.0;}
  //if(sunPos.y < 0.0){sunPos.y = 0.0;}

  //Extend the rays
  vec4 finalColor = vec4(0.0);
  vec2 deltaTc = oUv - sunPos;
  vec2 curTc = oUv;
  deltaTc *= 1.0 / float(uSamples) * uDensity;
  float illumDecay = 1.0;

  for(int i=0;i<uSamples;i++)
  {
    curTc -= deltaTc;
    vec4 color = texture2D(uScreenTexture,curTc);
    color *= illumDecay * uWeight;

    finalColor += color;
    illumDecay *= uDecay;
  }

  return finalColor * uExposure;
}

void main()
{
	color = GetColor(); 
}

