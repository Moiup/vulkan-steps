# **Step 10.2: Shader Modules (9.1)**
## **Hints**
There are two things to do, create a Shader Module with [`vkCreateShaderModule`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap9.html#vkCreateShaderModule) and then fill a [`VkPipelineShaderStageCreateInfo`]() structure.

Of course, we have to make it twice: once for the vertex shader, and once for the fragment shader.

## **The Code**

At the end of the creation of the two [`VkPipelineShaderStageCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap10.html#VkPipelineShaderStageCreateInfo), create an array of [`VkPipelineShaderStageCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap10.html#VkPipelineShaderStageCreateInfo) where you store a the two structures created for each of the shaders.

Do not forget to destroy the shader module at the end with: [`vkDestroyShaderModule`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap10.html#VkPipelineShaderStageCreateInfo)

```C++
    /**************************************************************/
	/* Step 10: Shaders (9.)                                      */
	/**************************************************************/
	/**************************************************************/
	/* Step 10.2: Shader Modules (9.1)                            */
	/**************************************************************/
	// Vertex shader
	std::vector<char> vertex_shader_text = readFile("shaders/vert.spv");
	VkShaderModule vertex_shader_module = nullptr;
	VkShaderModuleCreateInfo vertex_shader_module_info{};
	vertex_shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertex_shader_module_info.pNext = nullptr;
	vertex_shader_module_info.flags = 0;
	vertex_shader_module_info.codeSize = vertex_shader_text.size();
	vertex_shader_module_info.pCode = reinterpret_cast<const uint32_t*>(vertex_shader_text.data());

	vk_result = vkCreateShaderModule(
		logical_device,
		&vertex_shader_module_info,
		nullptr,
		&vertex_shader_module
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating vertex shader module!");
	}
	else {
		std::cout << "Vertex shader module created." << std::endl;
	}

	VkPipelineShaderStageCreateInfo vertex_pipeline{};
	vertex_pipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertex_pipeline.pNext = nullptr;
	vertex_pipeline.flags = 0;
	vertex_pipeline.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertex_pipeline.module = vertex_shader_module;
	vertex_pipeline.pName = "main";
	vertex_pipeline.pSpecializationInfo = nullptr;

	// Fragment shader
	std::vector<char> fragment_shader_text = readFile("shaders/frag.spv");
	VkShaderModule fragment_shader_module = nullptr;
	VkShaderModuleCreateInfo fragment_shader_module_info{};
	fragment_shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragment_shader_module_info.pNext = nullptr;
	fragment_shader_module_info.flags = 0;
	fragment_shader_module_info.codeSize = fragment_shader_text.size();
	fragment_shader_module_info.pCode = reinterpret_cast<const uint32_t*>(fragment_shader_text.data());

	vk_result = vkCreateShaderModule(
		logical_device,
		&fragment_shader_module_info,
		nullptr,
		&fragment_shader_module
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating fragment shader module!");
	}
	else {
		std::cout << "Fragment shader created." << std::endl;
	}

	VkPipelineShaderStageCreateInfo fragment_pipeline{};
	fragment_pipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragment_pipeline.pNext = nullptr;
	fragment_pipeline.flags = 0;
	fragment_pipeline.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragment_pipeline.module = fragment_shader_module;
	fragment_pipeline.pName = "main";
	fragment_pipeline.pSpecializationInfo = nullptr;

	// Putting both pipeline into an array
	std::vector<VkPipelineShaderStageCreateInfo> shader_stage_arr = {
		vertex_pipeline,
		fragment_pipeline
	};
	std::cout << "Shaders created." << std::endl;

```

File: [step_10_2_shader_modules.cpp](../Code/step_10_2_shader_modules.cpp)

| Previous | Next |
|---|---|
| [Step 10.1: Compiling and reading shaders](compiling_and_reading_shaders.md) | [Step 11: Framebuffer (8.3.)](framebuffer.md) |