# **Step 10.1: Compiling and reading shaders**

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


| Previous | Next |
|---|---|
| [Step 10: Shader](shader.md) | [Step 10.2: Shader Modules (9.1)](shader_modules.md) |