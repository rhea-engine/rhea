#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texcoord;

layout(std140) uniform cbPerFrame
{
    mat4 V;
    mat4 P;
    mat4 VP;
    vec4 cameraPosition;
};

layout(std140) uniform cbPerObject
{
    mat4 worldMatrix;
    mat4 invModel;
};

out vec3 v_normal;
out vec2 v_tex;
out vec4 v_positionWS;

void main()
{
    vec4 position = vec4(a_position, 1.0);

    vec4 worldPos = position * worldMatrix;
    v_normal = normalize(a_normal * mat3(transpose(invModel)));
    gl_Position = vec4(worldPos.xyz, 1.0) * VP;
    v_tex = a_texcoord;
    v_positionWS = vec4(worldPos.xyz, 1.0);
}