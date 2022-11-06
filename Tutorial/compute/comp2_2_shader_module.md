# **Comp 2.2: Shader Modules (9.1)**
## **Hints**
You will need to fill a [`VkPipelineShaderStageCreateInfo`]() structure.

## **The Code**

There are two things to do, create a Shader Module with [`vkCreateShaderModule`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap9.html#vkCreateShaderModule) which require to fill a [`VkShaderModuleCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap9.html#VkShaderModuleCreateInfo) and then fill a [`VkPipelineShaderStageCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap10.html#VkPipelineShaderStageCreateInfo) structure.

Do not forget to destroy the shader module at the end with: [`vkDestroyShaderModule`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap10.html#VkPipelineShaderStageCreateInfo)

```C++
    /**************************************************************/
	/* Comp2: Compute Shader                                      */
	/**************************************************************/
	/**************************************************************/
	/* Comp 2.2: Shader Module (9.1)                              */
	/**************************************************************/
	std::vector<char> compute_shader_text = readFile("../shaders/compute.spv");
	VkShaderModule compute_shader_module = nullptr;
	VkShaderModuleCreateInfo compute_shader_module_info{};
	compute_shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	compute_shader_module_info.pNext = nullptr;
	compute_shader_module_info.flags = 0;
	compute_shader_module_info.codeSize = compute_shader_text.size();
	compute_shader_module_info.pCode = reinterpret_cast<const uint32_t*>(compute_shader_text.data());

	vk_result = vkCreateShaderModule(
		logical_device,
		&compute_shader_module_info,
		nullptr,
		&compute_shader_module
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating compute shader module!");
	}
	else {
		std::cout << "Compute shader module created." << std::endl;
	}

	VkPipelineShaderStageCreateInfo compute_shader_stage{};
	compute_shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	compute_shader_stage.pNext = nullptr;
	compute_shader_stage.flags = 0;
	compute_shader_stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	compute_shader_stage.module = compute_shader_module;
	compute_shader_stage.pName = "main";
	compute_shader_stage.pSpecializationInfo = nullptr;

	std::cout << "Compute Shader created." << std::endl;

```

File: [comp2_2_shader_module.cpp](../../Code/comp2_2_shader_module.cpp)

| Previous | Next |
|---|---|
| [Comp 2.1: Compiling](comp2_1_compiling.md) | [Comp 3: Storage Buffers](comp3_descriptor_sets.md) |