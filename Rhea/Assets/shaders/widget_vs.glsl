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

layout(std140) uniform cbBackground
{
    vec2 tiling;
    vec2 scaling;
};

out vec2 v_tex;

void main()
{
    vec4 position = vec4(a_position, 1.0);

    mat4 vp = transpose(VP);
    mat4 model = transpose(world_matrix);

    position.x += world_matrix[0][3];
    position.y += world_matrix[1][3];

    gl_Position = position * VP;

    vec2 texTransform = vec2(
        (tiling.x > 1.0) ? tiling.x : scaling.x,
        (tiling.y > 1.0) ? tiling.y : scaling.y
    );

    v_tex = a_texcoord * texTransform;   
}