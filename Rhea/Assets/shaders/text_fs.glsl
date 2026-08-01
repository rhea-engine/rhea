#version 330 core

in vec2 v_tex;
out vec4 FragColor;

uniform sampler2D ObjTexture;

layout(std140) uniform cbMaterial
{
    vec4 mat_diffuse;
};

void main()
{
	float coverage = texture(ObjTexture, v_tex).r;
	if (coverage == 0.0)
		discard;

	FragColor = vec4(mat_diffuse.rgb, mat_diffuse.a * coverage);
}
