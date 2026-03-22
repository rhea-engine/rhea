#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_texcoord; // not used, but kept for layout compatibility

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

out vec3 v_tex;

void main()
{
    vec4 position = vec4(a_position, 1.0);
    vec4 worldPos = position * world_matrix;

    vec4 clipPos = worldPos * VP;
    gl_Position = clipPos.xyww;
    v_tex = a_position;
}