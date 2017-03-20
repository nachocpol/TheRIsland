/*
    Export a normal map texture
    and sample it from the fragment.
    So we could reduce the res of the 
    terrain mesh and have a lot of detail
    with the normal.
*/
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
in DATA 
{
    vec2 texCoord;
    vec3 pos;
    vec3 lPos;
    vec3 normal;
    mat3 TBN;
    vec4 posLightSpace;
}vs_in;

//Uniforms
layout(std140)uniform uFogData
{
    vec4 uFogColorEnabled; //x,y,z:color a:enabled
    vec4 uFogStartEndMode; //x:start y:end z:mode
};
uniform vec3 uCamPos;
uniform sampler2D uShadowMap;
uniform sampler2D uGrassTexture;
uniform sampler2D uGravelTexture;
uniform sampler2D uSnowTexture;
uniform sampler2D uCliffTexture;
uniform sampler2D uGrassNormal;
uniform sampler2D uCliffNormal;

uniform int uCurLights;
uniform sampler2D uShadowMaps[MAX_LIGHTS];

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

float GetSlope()
{
    vec3 up = vec3(0,1,0);          //Define an up vector
    float f = dot(up,vs_in.normal); //Find the slope factor
    f = pow(f,15);                  //Make the slopes more pronunciated
    return f;
}

vec2 GetTexCoord()
{
    //Build the texture coord (parallax mapping)
    float cliffHeight = 1.0 - texture(uCliffNormal,vs_in.texCoord).a;
    float grassHeight = 1.0 - texture(uGrassNormal,vs_in.texCoord).a;
    float height =  mix(cliffHeight,grassHeight,GetSlope());
    float heightScale = 0.1;

    //Set view pos and frag pos in tangent space
    vec3 camPos = vs_in.TBN * uCamPos;
    vec3 verPos = vs_in.TBN * vs_in.pos;
    vec3 eDir = normalize(camPos-verPos);
    vec2 uv = eDir.xy / eDir.z * (height * heightScale);
    return vs_in.texCoord - uv;
    
    //return vs_in.texCoord;
}

vec3 GetNormal(vec2 tc)
{
    vec3 cliffNorm = texture(uCliffNormal,tc).rgb;
    cliffNorm = normalize(cliffNorm * 2.0 - 1.0);
    cliffNorm = normalize(vs_in.TBN * cliffNorm);

    vec3 grassNorm = texture(uGrassNormal,tc).rgb;
    grassNorm = normalize(grassNorm * 2.0 - 1.0);
    grassNorm = normalize(vs_in.TBN * grassNorm);

    return mix(cliffNorm,grassNorm,GetSlope());
}

vec3 GetTextureColor(vec2 tc)
{ 
    vec3 cliffColor = texture(uCliffTexture,tc).rgb;
    vec3 grassColor = texture(uGrassTexture,tc).rgb;
    
    return mix(cliffColor,grassColor,GetSlope());
}

float CalcShadow(vec4 posLSpace,int id,vec3 normal)
{
    vec3 projCoords = posLSpace.xyz / posLSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(uShadowMaps[id], projCoords.xy).r; 
    float currentDepth = projCoords.z;

    //Calculate bias
    float bias = max(0.05 * (1.0 - dot(normal,uLights[id].lDir)), 0.005);      

    //PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMaps[id], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uShadowMaps[id], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }

    shadow /= 9.0;
    return shadow;
}

vec4 GetColor()
{
    vec2 tc = GetTexCoord();
    vec4 lightCol = vec4(1.0);
    vec3 normal = GetNormal(tc);

    //Diffuse
    vec3 lDir = -uLights[0].lDir;
    float dif = max(dot(normal,lDir),0.0);
    vec4 color = vec4(GetTextureColor(tc),1.0);

    //Ambient
    vec3 ambColor = vec3(0.9,0.5,0.5);
    float ambf = 0.3f;
    vec4 ambient = vec4(ambColor * ambf,1.0);

    //Calc shadow
    float shadow = 1.0 - CalcShadow(uLights[0].lMatrix * vec4(vs_in.pos,1.0),0,normal);

    //return vec4(dif);
    return (ambient + (lightCol*dif) )*color;
}

void main()
{
    oColor = GetColor();
    oColor = mix(oColor,vec4(uFogColorEnabled.xyz,1.0),GetFog());
    //oColor = vec4(GetNormal(),1.0);
    //oColor = vec4(vs_in.normal,1.0);
    //oColor = vec4(vs_in.tang,1.0);
    //oColor = vec4(vs_in.bitang,1.0);
    //oColor = vec4(dif,dif,dif,1.0);
    //oColor = vec4(GetColor(),1.0);
}