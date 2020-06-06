cbuffer cbPerObject
{
	float4x3 model;
	float4x4 VP;	
};

Texture2D Texture[5];
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
	
	return totalColor;
}
