#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;
layout(location = 3) in vec3 a_tangent;

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

out vec3 v_normal;
out vec2 v_tex;
out vec4 v_positionWS;
out vec3 v_tangent;
out vec3 v_bitangent;

void main()
{
    vec4 position = vec4(a_position, 1.0);

    vec4 worldPos = position * world_matrix;
    v_normal = normalize(a_normal * mat3(transpose(inverse_matrix)));
    gl_Position = vec4(worldPos.xyz, 1.0) * VP;
    v_tex = a_texcoord;
    v_positionWS = vec4(worldPos.xyz, 1.0);

    v_tangent = normalize(a_tangent * mat3(world_matrix));
    v_bitangent = normalize(cross(v_tangent, v_normal));
}