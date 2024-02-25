cbuffer cbPerFrame
{
    float4x4 V;
    float4x4 P;
    float4x4 VP;
};

cbuffer cbPerObject
{
    float4x4 model;
};

struct SkymapVSOutput
{
    float4 position : SV_POSITION;
    float3 tex : TEXCOORD;
};

TextureCube SkyMap : register(t0);
SamplerState ObjSamplerState;

SkymapVSOutput VShader(float4 position : POSITION, float2 tex : TEXCOORD)
{    
    SkymapVSOutput output = (SkymapVSOutput) 0;
    
    float4 worldPos = mul(position, model);
    output.position = mul(worldPos, VP).xyww;    
    output.tex = position;

    return output;
}

float4 PShader(SkymapVSOutput input) : SV_Target
{
    return SkyMap.Sample(ObjSamplerState, input.tex);
}