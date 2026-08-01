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

cbuffer cbMaterial : register(b3)
{
    float4 mat_diffuse;
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
        
    // Change the position vector to be 4 units for proper matrix calculations.
    position.w = 1.0f;  
    
    position.x = position.x + model[3][0];
    position.y = position.y + model[3][1];

    output.position = mul(position, VP);
    output.tex = tex;
	
    return output;
}


float4 PShader(VS_OUTPUT input) : SV_TARGET
{
    float coverage = ObjTexture.Sample(ObjSamplerState, input.tex).r;
    return float4(mat_diffuse.rgb, mat_diffuse.a * coverage);
}
