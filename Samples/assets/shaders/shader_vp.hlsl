cbuffer cbPerObject
{
	float4x4 VP;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT VShader(float4 inPos : POSITION, float4 inColor : COLOR)
{
	VS_OUTPUT output;

	output.Pos = mul(inPos, VP);
	output.Color = inColor;

	return output;
}

float4 PShader(VS_OUTPUT input) : SV_TARGET
{
	return input.Color;
}