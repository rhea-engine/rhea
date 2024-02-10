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

cbuffer cbPerFrame
{
    float4x4 V;
    float4x4 P;
    float4x4 VP;
    float3 cameraPosition;
};

cbuffer cbPerObject
{
    float4x3 model;
};

cbuffer cbLight : register(b2)
{
    Light lights[8];
    float4 ambient;
    float lightCount;
};

Texture2D Texture[5];
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
    float3 normal : NORMAL;
	float2 tex : TEXCOORD;
    float4 positionWS : TEXCOORD1;
};

float3x3 InverseTranspose(float4x3 M)
{
    float3 a = M[0].xyz;
    float3 b = M[1].xyz;
    float3 c = M[2].xyz;

    float3x3 result;
    result[0] = cross(b, c);
    result[1] = cross(c, a);
    result[2] = cross(a, b);

    float det = dot(a, result[0]);
    return transpose(result) / det;
}

VS_OUTPUT VShader(float4 position : POSITION, float3 normal : NORMAL, float2 tex : TEXCOORD)
{
	VS_OUTPUT output;

	float3 worldPos = mul(position, model);
    output.normal = normalize(mul(normal, (float3x3) transpose(InverseTranspose(model))));
	output.position = mul(float4(worldPos, 1.0), VP);
	output.tex = tex;
    output.positionWS = float4(worldPos, 1.0);

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
 
    return light.Color * pow(RdotV, 32 /*Material.SpecularPower*/);
}

float4 DoDiffuse(Light light, float3 L, float3 N)
{
    float NdotL = max(0, dot(N, L));
    return light.Color * NdotL;
}

LightingResult DoDirectionalLight(Light light, float3 View, float4 P, float3 N)
{
    LightingResult result;
 
    float3 L = -light.Direction.xyz;
 
    result.Diffuse = DoDiffuse(light, L, N);
    result.Specular = DoSpecularPhong(light, View, L, N);
 
    return result;
}

float DoAttenuation(Light light, float d)
{
    float att = (light.ConstantAttenuation + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d);
    if (att == 0.0f)
        return 1.0f;
    return 1.0f / att;
}

LightingResult DoPointLight(Light light, float3 V, float4 P, float3 N)
{
    LightingResult result;
 
    float3 L = normalize(light.Position - P).xyz;
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
 
    float3 L = normalize(light.Position - P).xyz;
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
    LightingResult single_result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
    LightingResult result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
    
    float3 View = normalize(cameraPosition - P.xyz);
    
    for (int i = 0; i < lightCount; i++)
    {
        
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
	float4 blendColor = Texture[0].Sample(ObjSamplerState, input.tex);
	// black means background
	float bgAmount = 1 - (blendColor.x + blendColor.y + blendColor.z);
	float2 tiledCoord = input.tex * 256.0;
	// get colors of texs
	float4 tex1col = Texture[1].Sample(ObjSamplerState, tiledCoord) * bgAmount;
	float4 tex2col = Texture[2].Sample(ObjSamplerState, tiledCoord) * blendColor.x;
	float4 tex3col = Texture[3].Sample(ObjSamplerState, tiledCoord) * blendColor.y;
	float4 tex4col = Texture[4].Sample(ObjSamplerState, tiledCoord) * blendColor.z;

	float4 totalColor = tex1col + tex2col + tex3col + tex4col;
	    
    LightingResult lit = ComputeLighting(input.positionWS, input.normal);
     
    float3 finalColor = (ambient.rgb + lit.Diffuse.rgb + lit.Specular.rgb) * totalColor.rgb;
    
    return float4(finalColor, totalColor.a);
}
