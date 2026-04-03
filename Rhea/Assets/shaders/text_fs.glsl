#version 330 core

in vec2 v_tex;
out vec4 FragColor;

uniform sampler2D ObjTexture;

void main()
{
    vec4 color = texture(ObjTexture, v_tex);

    // Match existing HLSL logic.
    if (color.r == 0.0)
    {
        //color.a = 0.0;
        discard;
    }

    FragColor = color;
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}