cbuffer cbPerFrame
{
    float4x4 V;
    float4x4 P;
	float4x4 VP;	
};

cbuffer cbPerObject : register(b1)
{
    float4x4 worldMatrix;
    float4x4 invModel;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT VShader(float4 inPos : POSITION, float4 inColor : COLOR)
{
	VS_OUTPUT output;

    float3 worldPos = mul(inPos, worldMatrix);
    output.Pos = mul(float4(worldPos, 1.0), VP);
	
//	output.Pos = mul(inPos, VP);
	output.Color = inColor;

	return output;
}

float4 PShader(VS_OUTPUT input) : SV_TARGET
{
	return input.Color;
}