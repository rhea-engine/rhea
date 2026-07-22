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
    
     // w=0: ignore translations in the View-Matrix.
    float4 clipPos = mul(float4(position.xyz, 0.0f), VP);

    // set Skybox to the farthest depth.
    output.position = clipPos.xyww;
    output.tex = position.xyz;

    return output;
}

float4 PShader(SkymapVSOutput input) : SV_Target
{
    return SkyMap.Sample(ObjSamplerState, input.tex);
}