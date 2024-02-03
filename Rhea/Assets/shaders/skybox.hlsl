cbuffer cbPerFrame
{
    float4x4 V;
    float4x4 P;
    float4x4 VP;
};

cbuffer cbPerObject
{
//    float4x4 WVP;
    float4x3 model;
};

struct SkymapVSOutput
{
    float4 position : SV_POSITION;
    float3 tex : TEXCOORD;
};

TextureCube SkyMap : register(t0);
SamplerState ObjSamplerState;

SkymapVSOutput VShader(float3 position : POSITION, float2 tex : TEXCOORD)
{    
    SkymapVSOutput output = (SkymapVSOutput) 0;
    
    float3 worldPos = mul(float4(position, 1.0), model);
    output.position = mul(float4(worldPos, 1.0), VP).xyww;    
    output.tex = position;

    return output;
}

float4 PShader(SkymapVSOutput input) : SV_Target
{
    return SkyMap.Sample(ObjSamplerState, input.tex);
}