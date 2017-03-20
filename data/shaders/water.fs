#version 330 core

#define MAX_LIGHTS 5
#define PI 3.14159265359
#define GAMMA 2.2

uniform struct Light
{
	vec4 lColor;   //lColor.a = lType//0:sun,1:spot
	vec3 lDir;
	mat4 lMatrix;
    vec3 lPosition;
    float lCutOff;
}uLights[MAX_LIGHTS];

//Inputs
in vec2 tc;
in vec3 norm;
in vec3 pos;
in vec4 clipSpace;

//Uniforms
layout(std140)uniform uFogData
{
    vec4 uFogColorEnabled; //x,y,z:color a:enabled
    vec4 uFogStartEndMode; //x:start y:end z:mode
};
uniform sampler2D uDudv;
uniform sampler2D uNorm;
uniform sampler2D uReflectionTex;
uniform sampler2D uRefractionTex;
uniform sampler2D uDepthTex;
uniform sampler2D uFoamTex;
uniform vec3 uCamPos;
uniform float uTime;
uniform int uCurLights;
uniform sampler2D uShadowMaps[MAX_LIGHTS];

//Shader constants
const float wStre = 0.08;//The ammount of distorsion
const float wSpeed = 0.02;
const float specPower = 190.0;
const float reflectPower = 0.9;

const float cNear = 0.01;
const float cFar = 500.0;

//Final fragment color
out vec4 color;

float GetFog()
{
    if(uFogColorEnabled.a == 0)
    {
        return 0.0;
    }
    float fogEnd = uFogStartEndMode.y;
    float fogStart = uFogStartEndMode.x;

    float curFog = abs(gl_FragCoord.z/gl_FragCoord.w);
    float fogFactor = 1.0-clamp( (fogEnd-curFog)/(fogEnd-fogStart), 0.0, 1.0);
    return fogFactor;
}

float CalcFresnel()
{
	vec3 toView = normalize(uCamPos - pos);
	float f = max(dot(toView,norm),0.0);
	return f;
}

vec3 CalcNorm()
{
	vec3 fNorm;
	vec4 nMap = texture(uNorm,vec2(tc.x+uTime*wSpeed,tc.y));
	vec4 nMap2= texture(uNorm,vec2(-tc.x+uTime*wSpeed,tc.y));
	nMap += nMap2;
	fNorm.x = nMap.r * 2 - 1;
	fNorm.y = nMap.b;
	fNorm.z = nMap.g * 2 - 1;
	return normalize(fNorm);
}

vec4 GetFoamColor()
{
	float foamSpeed = wSpeed * 0.25f;
	vec4 foamColor = texture(uFoamTex,vec2(tc.x+uTime*foamSpeed,tc.y)*5.0);
	return foamColor;
}

vec4 GetColor()
{
	//Perfom clip space transformation (normalized device)
	vec2 ctc = vec2(clipSpace.xy)/clipSpace.w;
	//Set in uv space (00,11)
	ctc = ctc * 0.5 + 0.5;
	vec2 reflectTc = ctc;
	vec2 refractTc = ctc;

	//Get depth value
	float depth = texture(uDepthTex,refractTc).r;
	float curDepth = gl_FragCoord.z;

	//Calculate distances (water-floor-camera)
	float dCamFloor = 2.0 * cNear * cFar /  ( cFar + cNear - (2.0 * depth - 1.0)*(cFar - cNear));
	float dCamWat = 2.0 * cNear * cFar /  ( cFar + cNear - (2.0 * curDepth - 1.0)*(cFar - cNear));
	float dWatFloor = dCamFloor - dCamWat;

	//Get dudv displacement (we reduce the displacement at the edges)
	vec2 disp = (texture(uDudv,vec2(tc.x+uTime*wSpeed,tc.y)).rg * 2.0 - 1.0) * wStre * clamp(dWatFloor / 5.0,0.0,1.0);
	vec2 disp2= (texture(uDudv,vec2(-tc.x+uTime*wSpeed,tc.y)).rg * 2.0 - 1.0) * wStre * clamp(dWatFloor / 5.0,0.0,1.0);
	disp += disp2;

	//Apply the displacement
	reflectTc += disp;
	refractTc += disp;

	//Clamp to remove glichy edges
	refractTc = clamp(refractTc,0.001,0.999);
	reflectTc.y = clamp(reflectTc.y,0.001,0.999);
	reflectTc.x = clamp(reflectTc.x,0.001,0.999);

	vec4 reflectColor = texture(uReflectionTex,vec2(reflectTc.x,-reflectTc.y));
	vec4 refractColor = texture(uRefractionTex,vec2(refractTc.x,refractTc.y));

	//Aply refraction & reflection colors
	vec4 fColor = mix(reflectColor,refractColor,CalcFresnel());
	fColor = mix(fColor,vec4(0.03, 0.03, 0.09,1.0),0.25);

	//Apply specular 
	vec3 toView = normalize(uCamPos - pos);
	vec3 fromLight = normalize(-uLights[0].lDir);
	vec3 reflect = normalize(reflect(fromLight,CalcNorm()));
	float spec = max(dot(reflect,toView),0.0);
	spec = pow(spec,specPower);
	vec3 specColor = vec3(1.0,1.0,1.0) * spec * reflectPower;
	fColor += vec4(specColor,0.0);

	//Apply depth corrections (add alpha on the edges)
	fColor.a = clamp(dWatFloor / 1.0,0.0,1.0);

	//Add foam on the edges
	float foamOff = (sin(uTime*2.0)+1.5)*0.1;
	if(dWatFloor >= foamOff && dWatFloor < 0.15+foamOff)
	{
		fColor = mix(fColor,GetFoamColor(),0.7);
		//fColor = mix(fColor,vec4(1.0),0.5);
		fColor.a = clamp(dWatFloor / 1.0,0.4,1.0);
	}

	return fColor;
}
void main()
{
	color = GetColor();
	color = mix(color,vec4(uFogColorEnabled.xyz,1.0),GetFog());
}