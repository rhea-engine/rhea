#version 330 core

in vec2 v_tex;
out vec4 FragColor;

layout(std140) uniform cbMaterial
{
    vec4 mat_diffuse;
    vec4 mat_ambient;
    vec4 mat_specular;
    vec4 mat_emissive;
    float mat_shininess;
    float has_normal_map;
    int num_textures;
    float _cbMaterial_pad0; // padding for std140 alignment
};

uniform sampler2D ObjTexture;

void main()
{
    vec4 texColor = texture(ObjTexture, v_tex);

    float useTexture = (num_textures > 0) ? 1.0 : 0.0;
    vec4 outColor = mix(mat_diffuse, texColor, useTexture);

    if (outColor.a <= 0.01)
        discard;

    FragColor = outColor;
    //FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}