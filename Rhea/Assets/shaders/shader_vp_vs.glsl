#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec4 a_color;

//uniform mat4 V;
//uniform mat4 P;
//uniform mat4 VP;

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

layout(std140) uniform cbBackground
{
    vec2 tiling;
    vec2 scaling;
};

out vec4 Color;

void main()
{
    gl_Position = VP * world_matrix * vec4(a_position, 1.0);
    Color = a_color;
}