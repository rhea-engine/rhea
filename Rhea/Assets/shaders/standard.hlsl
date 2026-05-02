// Light types.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
    
struct Light
{
    float3 Direction;
    float Type;
    float4 Position;
    float4 Color;
    float4 Diffuse;
    float4 Specular;
    
    float SpotAngle;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
};

cbuffer cbPerFrame : register(b0)
{
    float4x4 V;
    float4x4 P;
    float4x4 VP;
    float4 cameraPosition;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 worldMatrix;
    float4x4 invModel;
};

cbuffer cbLight : register(b2)
{
    Light lights[8];
    float4 ambient;
    float lightCount;
};

cbuffer cbMaterial : register(b3)
{
    float4 mat_diffuse;
    float4 mat_ambient;
    float4 mat_specular;
    float4 mat_emissive;
    float mat_shininess;
    float has_normal_map;
}

Texture2D BaseColorTexture;
Texture2D NormalMap;
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
    float3 normal : NORMAL;
	float2 tex : TEXCOORD;
    float4 positionWS : TEXCOORD1;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

VS_OUTPUT VShader(float4 position : POSITION, float3 normal: NORMAL, float2 tex : TEXCOORD, float3 tangent : TANGENT)
{
	VS_OUTPUT output;

	float3 worldPos = mul(position, worldMatrix);
    output.normal = normalize(mul(normal, (float3x3)transpose(invModel)));
	output.position = mul(float4(worldPos, 1.0), VP);
	output.tex = tex;
    output.positionWS = float4(worldPos, 1.0);
    
    output.tangent = normalize(mul(tangent, (float3x3) worldMatrix)).xyz;
    // Compute binormal (cross product of normal and tangent)
    output.bitangent = normalize(cross(output.tangent, output.normal));
    
	return output;
}

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

float4 DoSpecularPhong(Light light, float3 V, float3 L, float3 N)
{
    // Phong lighting.
    float3 R = normalize(reflect(-L, N));
    float RdotV = max(0, dot(R, V));

    return light.Color * pow(RdotV, mat_shininess);   
}

float4 DoSpecularBlinnPhong(Light light, float3 V, float3 L, float3 N)
{
    // Blinn-Phong lighting
    float3 H = normalize(L + V);
    float NdotH = max(0, dot(N, H));

    return light.Color * pow(NdotH, mat_shininess);
}

float4 DoDiffuse(Light light, float3 L, float3 N)
{
    float NdotL = max(0, dot(N, L));
    return light.Color * NdotL;
}

LightingResult DoDirectionalLight(Light light, float3 View, float4 P, float3 N)
{
    LightingResult result;
 
    float3 L = normalize(-light.Direction.xyz);
    N = normalize(N);
    View = normalize(View);
 
    result.Diffuse = DoDiffuse(light, L, N);
    result.Specular = DoSpecularPhong(light, View, L, N);
 
    return result;
}

float DoAttenuation(Light light, float distance)
{
    float att = (light.ConstantAttenuation + light.LinearAttenuation * distance + light.QuadraticAttenuation * distance * distance);
    if (att == 0.0f)
        return 0.0f;
    float result = 1.0f / att;
    return result;
}

LightingResult DoPointLight(Light light, float3 V, float4 P, float3 N)
{
    LightingResult result;
 
    float3 L = (light.Position - P).xyz;
    float distance = length(L);
    L = L / distance;
 
    float attenuation = DoAttenuation(light, distance);
 
    result.Diffuse = DoDiffuse(light, L, N) * attenuation;
    result.Specular = DoSpecularPhong(light, V, L, N) * attenuation;   
    
    return result;
}

float DoSpotCone(Light light, float3 L)
{
    float minCos = cos(light.SpotAngle);
    float maxCos = (minCos + 1.0f) / 2.0f;
    float cosAngle = dot(light.Direction.xyz, -L);
    return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoSpotLight(Light light, float3 V, float4 P, float3 N)
{
    LightingResult result;
 
    //float3 L = normalize(light.Position - P).xyz;
    //float distance = length(L);
    //L = L / distance;
    float3 L = (light.Position.xyz - P.xyz);
    float distance = length(L);
    L = L / distance;
 
    float attenuation = DoAttenuation(light, distance);
    float spotIntensity = DoSpotCone(light, L);
 
    result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity;
    result.Specular = DoSpecularPhong(light, V, L, N) * attenuation * spotIntensity;
    
    return result;
}

LightingResult ComputeLighting(float4 P, float3 N)
{    
    LightingResult result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
    
    float3 View = normalize(cameraPosition - P).xyz;
    
    for (int i = 0; i < lightCount; i++)
    {
        LightingResult single_result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
        
        switch (lights[i].Type)
        {       
            case DIRECTIONAL_LIGHT:       
                single_result = DoDirectionalLight(lights[i], View, P, N);
                break;
            case POINT_LIGHT:
                single_result = DoPointLight(lights[i], View, P, N);
                break;
            case SPOT_LIGHT:
                single_result = DoSpotLight(lights[i], View, P, N);
                break;
        }
        result.Diffuse += single_result.Diffuse;
        result.Specular += single_result.Specular;
    }
    
    result.Diffuse = saturate(result.Diffuse);
    result.Specular = saturate(result.Specular);
    
    return result;
}

float4 PShader(VS_OUTPUT input) : SV_TARGET
{
    float3 finalNormal = input.normal;
    
    float4 diffuseSample = BaseColorTexture.Sample(ObjSamplerState, input.tex);
    
    if (has_normal_map > 0.0f)
    {
       // Sample normal map
        float3 normalMapSample = NormalMap.Sample(ObjSamplerState, input.tex).xyz * 2.0f - 1.0f;

        // Transform normal map sample to tangent space
        float3x3 TBN = float3x3(input.tangent, input.bitangent, input.normal);
        float3 normalMapSampleTangentSpace = mul(normalMapSample, TBN);

        // Apply normal map to interpolated normal
        finalNormal = normalize(normalMapSampleTangentSpace);
    }
    
    LightingResult lit = ComputeLighting(input.positionWS, finalNormal);   
    
    float3 finalColor = (
        mat_emissive.rgb +
        mat_ambient.rgb * ambient.rgb * diffuseSample.rgb +
        mat_diffuse.rgb * lit.Diffuse.rgb * diffuseSample.rgb +
        mat_specular.rgb * lit.Specular.rgb);

    return float4(finalColor, diffuseSample.a);
}
