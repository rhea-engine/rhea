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

    vec4 worldPos = worldMatrix * position;
    gl_Position = VP * worldPos;

    v_normal = normalize(mat3(transpose(invModel)) * a_normal);
    v_tex = a_texcoord;
    v_positionWS = worldPos;
}