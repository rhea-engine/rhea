// MIT License; Copyright (c) 2019-2026 Meworla.
cbuffer cbPerFrame : register(b0) {
    float4x4 V;
    float4x4 P;
    float4x4 VP;
};
cbuffer cbPerObject : register(b1) {
    float4x4 world_matrix;
    float4x4 inverse_matrix;
};
Texture2D sample_texture : register(t0);
SamplerState sample_sampler : register(s0);
struct VertexOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};
VertexOutput VShader(float3 position : POSITION, float4 color : COLOR, float2 uv : TEXCOORD) {
    VertexOutput output;
    output.position = mul(mul(float4(position, 1.0), world_matrix), VP);
    output.color = color;
    output.uv = uv;
    return output;
}
float4 PColor(VertexOutput input) : SV_TARGET { return input.color; }
float4 PTexture(VertexOutput input) : SV_TARGET { return sample_texture.Sample(sample_sampler, input.uv) * input.color; }
