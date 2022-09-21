#version 450

// IN
layout (location = 0) in vec4 color;

// OUT
layout (location = 0) out vec4 out_color;

void main(){
    out_color = color;
}
