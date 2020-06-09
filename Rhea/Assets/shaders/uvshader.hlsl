cbuffer cbPerFrame
{
    float4x4 V;
    float4x4 P;
    float4x4 VP;
};

cbuffer cbPerObject
{
    float4x3 model;
    float4x4 WVP;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

VS_OUTPUT VShader(float4 position : POSITION, float2 tex : TEXCOORD)
{
	VS_OUTPUT output;

	output.position = mul(position, WVP);
	output.tex = tex;

	return output;
}


float4 PShader(VS_OUTPUT input) : SV_TARGET
{
	return ObjTexture.Sample(ObjSamplerState, input.tex);
}
