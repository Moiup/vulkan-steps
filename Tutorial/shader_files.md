# **Shader files**

To draw the cube we need a vertex shader and a fragment shader.

## **The Code**
Vertex shader:
```C
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
```

Fragment shader:
```C
#version 450

// IN
layout (location = 0) in vec4 color;

// OUT
layout (location = 0) out vec4 out_color;

void main(){
    out_color = color;
}

```

Vertex shader: [`vert.vert`](../Code/shaders/vert.vert)
Vertex shader: [`frag.frag`](../Code/shaders/frag.frag)

| Previous | Next |
|---|---|
| [Environment Configuration](environment_configuration.md) | [Step 0: Window creation with GLFW](context_initialisation_with_GLFW.md) |