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
    float4 mat_ambient;
    float4 mat_specular;
    float4 mat_emissive;
    float mat_shininess;
    float has_normal_map;
    int num_textures;
}

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
    float4 texColor = ObjTexture.Sample(ObjSamplerState, input.tex);
    
    // Blend based on `useTexture` (avoids if statements)
    float4 outColor = lerp(mat_diffuse, texColor, (num_textures>0) ? 1.0f : 0.0f);

    // Discard fully transparent pixels
    clip(outColor.a - 0.01f);

    return outColor;
}
