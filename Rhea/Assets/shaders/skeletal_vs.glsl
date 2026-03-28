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

    // Skin position by transforming with each bone separately, then blending.
    vec4 p0 = position * boneMatrices[j0];
    vec4 p1 = position * boneMatrices[j1];
    vec4 p2 = position * boneMatrices[j2];
    vec4 p3 = position * boneMatrices[j3];

    vec4 skinnedPosition =
        p0 * w.x +
        p1 * w.y +
        p2 * w.z +
        p3 * w.w;

    // Skin normal/tangent the same way, using the 3x3 part of each bone matrix.
    vec3 n0 = a_normal * mat3(boneMatrices[j0]);
    vec3 n1 = a_normal * mat3(boneMatrices[j1]);
    vec3 n2 = a_normal * mat3(boneMatrices[j2]);
    vec3 n3 = a_normal * mat3(boneMatrices[j3]);

    vec3 t0 = a_tangent * mat3(boneMatrices[j0]);
    vec3 t1 = a_tangent * mat3(boneMatrices[j1]);
    vec3 t2 = a_tangent * mat3(boneMatrices[j2]);
    vec3 t3 = a_tangent * mat3(boneMatrices[j3]);

    vec3 skinnedNormal =
        n0 * w.x +
        n1 * w.y +
        n2 * w.z +
        n3 * w.w;

    vec3 skinnedTangent =
        t0 * w.x +
        t1 * w.y +
        t2 * w.z +
        t3 * w.w;

    vec4 worldPos = skinnedPosition * world_matrix;
    gl_Position = vec4(worldPos.xyz, 1.0) * VP;

    v_normal = normalize(skinnedNormal * mat3(transpose(inverse_matrix)));
    v_tangent = normalize(skinnedTangent * mat3(world_matrix));
    v_bitangent = normalize(cross(v_tangent, v_normal));

    v_tex = a_texcoord;
    v_positionWS = vec4(worldPos.xyz, 1.0);
}