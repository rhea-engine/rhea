#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texcoord;

layout(std140) uniform cbPerFrame
{
    mat4 V;
    mat4 P;
    mat4 VP;
    vec4 cameraPos;
};

layout(std140) uniform cbPerObject
{
    mat4 world_matrix;
    mat4 inverse;
};

out vec2 v_tex;

void main()
{
    vec4 position = vec4(a_position, 1.0);

    position.x += world_matrix[0][3];
    position.y += world_matrix[1][3];

    gl_Position = position * VP;
    v_tex = a_texcoord;
}