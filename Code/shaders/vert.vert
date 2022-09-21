#version 450

// IN
layout (location = 0) in vec3 coord;
layout (location = 1) in vec4 in_color;

// OUT
layout (location = 0) out vec4 out_color;

// UNIFORM BUFFER
layout (std140, binding = 0) uniform buf{
    mat4 mvp;
} ubuf_mvp;


void main(){
    out_color = in_color;
    gl_Position = ubuf_mvp.mvp * vec4(coord, 1.0);
}