cbuffer cbPerObject
{
	float4x3 model;
	float4x4 VP;	
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

	float3 worldPos = mul(position, model);
	output.position = mul(float4(worldPos, 1.0), VP);
	output.tex = tex;

	return output;
}


float4 PShader(VS_OUTPUT input) : SV_TARGET
{
	return ObjTexture.Sample(ObjSamplerState, input.tex);
}
