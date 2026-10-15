// MIT License; Copyright (c) 2019-2026 Meworla.
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;
layout(std140) uniform cbPerFrame { mat4 V; mat4 P; mat4 VP; };
layout(std140) uniform cbPerObject { mat4 world_matrix; mat4 inverse_matrix; };
out vec4 vertex_color;
out vec2 vertex_uv;
void main() {
    gl_Position = VP * world_matrix * vec4(position, 1.0);
    vertex_color = color;
    vertex_uv = uv;
}
