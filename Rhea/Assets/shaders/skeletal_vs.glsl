#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in uvec4 a_jointIndices;
layout(location = 5) in vec4 a_jointWeights;

layout(std140) uniform cbPerFrame
{
    mat4 V;
    mat4 P;
    mat4 VP;
    vec4 cameraPosition;
};

layout(std140) uniform cbPerObject
{
    mat4 world_matrix;
    mat4 inverse_matrix;
};

layout(std140) uniform cbBone
{
    mat4 boneMatrices[100];
};

out vec3 v_normal;
out vec2 v_tex;
out vec4 v_positionWS;
out vec3 v_tangent;
out vec3 v_bitangent;

void main()
{
   vec4 position = vec4(a_position, 1.0);

    vec4 jointWeights = a_jointWeights;
    float weightSum = jointWeights.x + jointWeights.y + jointWeights.z + jointWeights.w;

    mat4 skinningTransform = mat4(1.0);

    if (weightSum > 0.0)
    {
        jointWeights /= weightSum;
        skinningTransform = mat4(0.0);

        uvec4 ji = min(a_jointIndices, uvec4(99u));

        if (jointWeights.x > 0.0)
            skinningTransform += boneMatrices[ji.x] * jointWeights.x;
        if (jointWeights.y > 0.0)
            skinningTransform += boneMatrices[ji.y] * jointWeights.y;
        if (jointWeights.z > 0.0)
            skinningTransform += boneMatrices[ji.z] * jointWeights.z;
        if (jointWeights.w > 0.0)
            skinningTransform += boneMatrices[ji.w] * jointWeights.w;
    }

    vec4 skinnedPosition = position * skinningTransform;
    vec4 worldPos = skinnedPosition * world_matrix;
    gl_Position = vec4(worldPos.xyz, 1.0) * VP;
   
    vec3 skinnedNormal = mat3(skinningTransform) * a_normal;
    vec3 skinnedTangent = mat3(skinningTransform) * a_tangent;

    v_normal = normalize(skinnedNormal * mat3(transpose(inverse_matrix)));
    v_tex = a_texcoord;
    v_positionWS = vec4(worldPos.xyz, 1.0);

    v_tangent = normalize(skinnedTangent * mat3(world_matrix));
    v_bitangent = normalize(cross(v_tangent, v_normal));
}