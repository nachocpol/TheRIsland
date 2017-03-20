#version 330 core

//Inputs from vertex
in DATA
{
    vec2 texCoord;
    vec3 norm;
    vec3 localPos;
    vec3 pos;
    vec3 tang;
    vec3 bitan;
}fs_in;

//Uniforms
layout(std140)uniform uFogData
{
    vec4 uFogColorEnabled; //x,y,z:color a:enabled
    vec4 uFogStartEndMode; //x:start y:end z:mode
};
uniform sampler2D uGrassBilTex;
uniform sampler2D uGradientTex;

//Final fragment color
out vec4 oColor;

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

void main()
{
	vec4 gCol = texture(uGradientTex,fs_in.texCoord);
	vec4 tCol = texture(uGrassBilTex,fs_in.texCoord);
	float gradient = clamp(gCol.r,0.2,1.0);

	if(tCol.a < 0.1 || fs_in.localPos.y > 0.9)
	{
		discard;
	}
	oColor = tCol - vec4(0.0,0.2,0.0,0.0);// * gradient;
	oColor = mix(oColor,vec4(uFogColorEnabled.xyz,1.0),GetFog());
}