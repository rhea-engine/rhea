// MIT License; Copyright (c) 2019-2026 Meworla.
#version 330 core
in vec4 vertex_color;
in vec2 vertex_uv;
uniform sampler2D sample_texture;
out vec4 fragment_color;
void main() { fragment_color = texture(sample_texture, vertex_uv) * vertex_color; }
