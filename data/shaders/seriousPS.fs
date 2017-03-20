#version 430 core

#define MAX_LIGHTS 5

#define L_CONSTANT 1.0
#define L_LINEAR 0.09
#define L_QUADRATIC 0.032

#define PI 3.14159265359
#define GAMMA 2.2

vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845),
    vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554),
    vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507),
    vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367),
    vec2(0.14383161, -0.14100790)
    );

uniform struct Light
{
	vec4 lColor;   //lColor.a = lType//0:sun,1:spot
	vec3 lDir;
	mat4 lMatrix;
    vec3 lPosition;
    float lCutOff;
}uLights[MAX_LIGHTS];

struct Material
{
    vec4 albedo;
    vec3 specular;
    float roughness;
    vec3 normal;
};

struct Attributes
{
    vec3 position;
    vec3 normal;
    vec3 bitangent;
    vec3 tangent;
}curAttributes;

//PBR general inputs
uniform sampler2D uIntegratedFG;
uniform samplerCube uEnvCubeMap;

//PBR textures inputs
uniform sampler2D uAlbedo;
uniform sampler2D uSpecular;
uniform sampler2D uGloss;
uniform sampler2D uNormal;

//Uniforms
layout(std140)uniform uFogData
{
    vec4 uFogColorEnabled; //x,y,z:color a:enabled
    vec4 uFogStartEndMode; //x:start y:end z:mode
};
uniform vec3 uCamPos;
uniform vec3 uDiffuseMatColor;
uniform int uCurLights;
uniform sampler2D uShadowMaps[MAX_LIGHTS];

//Inputs from vertex
in DATA
{
    vec2 texCoord;
    vec3 norm;
    vec3 pos;
    vec3 tang;
    vec3 bitan;
    vec3 posLightSpace;
}fs_in;

//Current attributes
//Attributes ;

//Final fragment color
out vec4 oColor;

float random(vec3 seed, int i)
{
    vec4 seed4 = vec4(seed, i);
    float dot_product = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5453);
}

vec3 FinalGamma(vec3 color)
{
    return pow(color, vec3(1.0 / GAMMA));
}

vec4 GammaCorrectTexture(sampler2D t,vec2 uv)
{
    vec4 tCol = texture(t, uv);
    return vec4(pow(tCol.rgb, vec3(GAMMA)), tCol.a);
}

vec3 GammaCorrectTextureRGB(sampler2D tex, vec2 uv)
{
    vec4 tCol = texture(tex, uv);
    return vec3(pow(tCol.rgb, vec3(GAMMA)));
}

vec4 GetAlbedo()
{
    return GammaCorrectTexture(uAlbedo,fs_in.texCoord);
}

vec3 GetSpecular()
{
    vec3 s =  GammaCorrectTextureRGB(uSpecular, fs_in.texCoord);
    return s;
}

float GetGloss()
{
    float g =  GammaCorrectTextureRGB(uGloss, fs_in.texCoord).r;
    return g;
}

float GetRoughness()
{
    return 1.0 - GetGloss();
}

float FresnelSchlick(float f0, float fd90, float view)
{
    return f0 + (fd90 - f0) * pow(max(1.0 - view, 0.1), 5.0);
}

float Diffuse(Light light,Material mat,vec3 eyeDir)
{
    vec3 halfVector = normalize(light.lDir + eyeDir);
 
    float NdotL = max(dot(curAttributes.normal, light.lDir), 0.0);
    float LdotH = max(dot(light.lDir, halfVector), 0.0);
    float NdotV = max(dot(curAttributes.normal, eyeDir), 0.0);
 
    float energyBias = mix(0.0, 0.5, mat.roughness);
    float energyFactor = mix(1.0, 1.0 / 1.51, mat.roughness);
    float fd90 = energyBias + 2.0 * (LdotH * LdotH) * mat.roughness;
    float f0 = 1.0;
 
    float lightScatter = FresnelSchlick(f0, fd90, NdotL);
    float viewScatter = FresnelSchlick(f0, fd90, NdotV);
    
    return lightScatter * viewScatter * energyFactor;
}

vec3 Specular(Light light, Material material, vec3 eyeDir)
{
    vec3 h = normalize(light.lDir + eyeDir);
    float NdotH = max(dot(curAttributes.normal, h), 0.0);
    
    //Distribution
    float rough2 = max(material.roughness * material.roughness, 2.0e-3); // capped so spec highlights don't disappear
    float rough4 = rough2 * rough2;
    float d = (NdotH * rough4 - NdotH) * NdotH + 1.0;
    float D = rough4 / (PI * (d * d));
 
    // Fresnel
    vec3 reflectivity = material.specular;
    float fresnel = 1.0;
    float NdotL = clamp(dot(curAttributes.normal, light.lDir), 0.0, 1.0);
    float LdotH = clamp(dot(light.lDir, h), 0.0, 1.0);
    float NdotV = clamp(dot(curAttributes.normal, eyeDir), 0.0, 1.0);
    vec3 F = reflectivity + (fresnel - fresnel * reflectivity) * exp2((-5.55473 * LdotH - 6.98316) * LdotH);
 
    // geometric / visibility
    float k = rough2 * 0.5;
    float G_SmithL = NdotL * (1.0 - k) + k;
    float G_SmithV = NdotV * (1.0 - k) + k;
    float G = 0.25 / (G_SmithL * G_SmithV);
 
    return G * D * F;
}

vec3 RadianceIBLIntegration(float NdotV, float roughness, vec3 specular)
{
    vec2 preintegratedFG = texture(uIntegratedFG, vec2(roughness, 1.0 - NdotV)).rg;
    return specular * preintegratedFG.r + preintegratedFG.g;
}

vec3 IBL(Material material, vec3 eyeDir)
{
    float NdotV = max(dot(curAttributes.normal, eyeDir), 0.0);
 
    vec3 reflectionVector = normalize(reflect(-eyeDir, curAttributes.normal));
    float smoothness = 1.0f - material.roughness;
    float mipLevel = (1.0 - smoothness * smoothness) * 10.0;
    vec4 cs = textureLod(uEnvCubeMap, reflectionVector, mipLevel);
    vec3 result = pow(cs.xyz, vec3(GAMMA)) * RadianceIBLIntegration(NdotV, material.roughness, material.specular);
 
    vec3 diffuseDominantDirection = curAttributes.normal;
    float diffuseLowMip = 9.6;
    vec3 diffuseImageLighting = textureLod(uEnvCubeMap, diffuseDominantDirection, diffuseLowMip).rgb;
    diffuseImageLighting = pow(diffuseImageLighting, vec3(GAMMA));
 
    return result + diffuseImageLighting * material.albedo.rgb;
}

vec3 GetNormal()
{
    mat3 toWorld = mat3(curAttributes.tangent,curAttributes.bitangent, fs_in.norm);
    vec3 normalMap = texture(uNormal, fs_in.texCoord).rgb * 2.0 - 1.0;
    normalMap = normalize(toWorld * normalMap.rgb);
    return normalMap;
}

float GetShadow(vec4 lPos,int id)
{
    vec3 projCoords = lPos.xyz / lPos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(uShadowMaps[id], projCoords.xy).r; 
    float currentDepth = projCoords.z;

    //Calculate bias
    float bias = max(0.05 * (1.0 - dot(curAttributes.normal,uLights[id].lDir)), 0.005);      

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
    //return abs(shadow-1.0);
}

float GetSpotAttenuation(Light l)
{
    float at = 0.01;
    vec3 lightDir = normalize(l.lPosition - fs_in.pos);
    float theta = dot(lightDir, normalize(-l.lDir)); 
    
    if(theta > l.lCutOff) 
    {    
        // Attenuation
        float distance    = length(l.lPosition - fs_in.pos);
        at = 1.0f / (L_CONSTANT + L_LINEAR * distance + L_QUADRATIC * (distance * distance));    
    }
    return at;
}

void main()
{
    //Set the current attributes
    curAttributes.position = fs_in.pos;
    curAttributes.normal = fs_in.norm;
    curAttributes.tangent = fs_in.tang;
    curAttributes.bitangent = fs_in.bitan;

    curAttributes.normal = GetNormal();

    //Build the current material
    Material material;
    material.albedo = GetAlbedo();
    material.roughness = GetRoughness();
    material.specular = GetSpecular();

    vec3 eyeDir = normalize(uCamPos - curAttributes.position);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    vec3 lightcolor = vec3(0.0);
    float attenuation = 0.0;

    //Calc the diffuse and specula color
    for(int i=0;i<uCurLights;i++)
    {
        if(uLights[i].lColor.w == 0.0)
        {
            attenuation = 1.0;
        }
        if(uLights[i].lColor.w == 1.0)
        {
            attenuation += (1.0/uCurLights) * GetSpotAttenuation(uLights[i]);
        }
        lightcolor += uLights[i].lColor.xyz;
        float NdotL = clamp(dot(curAttributes.normal,uLights[i].lDir),0.0,1.0);

        diffuse  += NdotL * Diffuse(uLights[i],material,eyeDir) * uLights[i].lColor.xyz;
        specular += NdotL * Specular(uLights[i],material,eyeDir)* uLights[i].lColor.xyz;
    }

    //Calc the current visibility (shadow)
    float visibility = 1.0;
    for(int i=0;i<uCurLights;i++)
    {
        visibility -= (1.0 / uCurLights) * GetShadow(uLights[i].lMatrix * vec4(fs_in.pos,1.0),i);
        
    }
    visibility = clamp(visibility,0.1,1.0);

    vec3 color;
    color += material.albedo.rgb * diffuse * visibility;
    color += (specular + IBL(material,eyeDir))*visibility;
    color *= attenuation;
    //color += lightcolor*0.1;

    color  = FinalGamma(color);
    oColor = vec4(color,material.albedo.a);

    //DEBUG
    //oColor = vec4(curAttributes.normal,1.0);
    //oColor = material.albedo;
    //oColor = vec4(vec3(material.roughness),1.0);
    //oColor = vec4(material.specular,1.0);
    //oColor = vec4(specular,1.0);
    //oColor = vec4(IBL(material,eyeDir),1.0);
    //oColor = vec4(clamp(dot(curAttributes.normal,uLights[0].lDir),0.4,1.0));
    //oColor = vec4((specular + IBL(material,eyeDir)),1.0);
}

