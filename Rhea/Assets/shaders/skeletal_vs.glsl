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

    int j0 = clamp(int(a_jointIndices.x), 0, 99);
    int j1 = clamp(int(a_jointIndices.y), 0, 99);
    int j2 = clamp(int(a_jointIndices.z), 0, 99);
    int j3 = clamp(int(a_jointIndices.w), 0, 99);

    vec4 w = a_jointWeights;
    float weightSum = w.x + w.y + w.z + w.w;

    if (weightSum > 0.0)
        w /= weightSum;
    else
        w = vec4(1.0, 0.0, 0.0, 0.0);

    vec4 skinnedPosition =
          (boneMatrices[j0] * position) * w.x
        + (boneMatrices[j1] * position) * w.y
        + (boneMatrices[j2] * position) * w.z
        + (boneMatrices[j3] * position) * w.w;

    vec3 skinnedNormal =
          (mat3(boneMatrices[j0]) * a_normal) * w.x
        + (mat3(boneMatrices[j1]) * a_normal) * w.y
        + (mat3(boneMatrices[j2]) * a_normal) * w.z
        + (mat3(boneMatrices[j3]) * a_normal) * w.w;

    vec3 skinnedTangent =
          (mat3(boneMatrices[j0]) * a_tangent) * w.x
        + (mat3(boneMatrices[j1]) * a_tangent) * w.y
        + (mat3(boneMatrices[j2]) * a_tangent) * w.z
        + (mat3(boneMatrices[j3]) * a_tangent) * w.w;

    vec4 worldPos = world_matrix * skinnedPosition;
    gl_Position = VP * worldPos;

    v_normal = normalize(mat3(inverse_matrix) * skinnedNormal);
    v_tangent = normalize(mat3(world_matrix) * skinnedTangent);
    v_bitangent = normalize(cross(v_tangent, v_normal));

    v_tex = a_texcoord;
    v_positionWS = worldPos;
}