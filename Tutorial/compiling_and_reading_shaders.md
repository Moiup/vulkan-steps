# **Step 10.1: Compiling and reading shaders**

## **Writing shader**
The shaders used in our example.

## Vertex shader
```c
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

## Fragment shader
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

## **Compiling**
To compile, we use `glslc` that we found in the VulkanSDK folder, inside `Bin`. For windows users: `glslc.exe`

To compile, open up the command line, go to the folder containing your shader, and run:
 - For the vertex shader
```
C:/VulkanSDK/version_of_Vulkan/Bin/glslc.exe vert.vert -o vert.spv
```
 - For the fragment shader
```
C:/VulkanSDK/version_of_Vulkan/Bin/glslc.exe frag.frag -o frag.spv
```

## **Reading**
The function to read the content of the output file gotten after compilation.

```C++
static std::vector<char> readFile(const std::string& file_name) {
	size_t file_size{};
	std::vector<char> buffer;

	std::ifstream file(file_name, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Error: faile to open file: " + file_name);
	}

	file_size = (size_t)file.tellg();
	buffer.resize(file_size);

	file.seekg(0);
	file.read(buffer.data(), file_size);

	file.close();

	return buffer;
}
```

File: [step_10_1_compiling_and_reading.cpp](../Code/step_10_1_compiling_and_reading_shaders.cpp)

| Previous | Next |
|---|---|
| [Step 10: Shader](shader.md) | [Step 10.2: Shader Modules (9.1)](shader_modules.md) |