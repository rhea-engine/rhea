#version 330 core

in vec3 v_tex;
out vec4 FragColor;

uniform samplerCube SkyMap;

void main()
{
    FragColor = texture(SkyMap, v_tex);
}