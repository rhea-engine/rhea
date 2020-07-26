cbuffer cbPerFrame
{
    float4x4 V;
    float4x4 P;
    float4x4 VP;
};

cbuffer cbPerObject
{
    float4x3 model;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

VS_OUTPUT VShader(float4 position : POSITION, float2 tex : TEXCOORD)
{
    VS_OUTPUT output;

    position.x = position.x + model[3][0];
    position.y = position.y + model[3][1];

    output.position = mul(position, VP);
    output.tex = tex;
	
    return output;
}


float4 PShader(VS_OUTPUT input) : SV_TARGET
{
    return ObjTexture.Sample(ObjSamplerState, input.tex);
}
