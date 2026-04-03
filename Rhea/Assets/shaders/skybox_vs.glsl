#version 330 core

layout(location = 0) in vec3 a_position;

layout(std140) uniform cbPerFrame
{
    mat4 V;
    mat4 P;
    mat4 VP;
    vec4 cameraPosition;
};

out vec3 v_tex;

void main()
{
    vec4 position = vec4(a_position, 1.0);
   
    mat4 viewNoTranslation = mat4(mat3(V));

    vec4 clipPos = P * viewNoTranslation * position;
    gl_Position = clipPos.xyww;

    v_tex = a_position;
}