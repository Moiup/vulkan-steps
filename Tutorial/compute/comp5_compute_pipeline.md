# **Comp 5: Compute Pipeline (10.1.)**
## **Hints**
To create a compute pipeline, we need to call [`vkCreateComputePipelines`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkCreateComputePipelines).

## **The Code**

In order to create a compute pipeline, we must first fill in a [`VkComputePipelineCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#VkComputePipelineCreateInfo) structure. To do so, you will need the [`VkPipelineShaderStageCreateInfo`]() created in the [`Comp 2.2 Shader Module (9.1.)`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#VkPipelineShaderStageCreateInfo) step, as well as the [`VkPipelineLayout`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#VkPipelineLayout) created in [Comp 4.2: Pipeline Layouts (14.2.2.)](comp4_2_pipeline_layouts.md)

Finally, call [`vkCreateComputePipelines`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkCreateComputePipelines).

Do not forget to destroy the compute pipeline by calling [`vkDestroyPipeline`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkDestroyPipeline)

```C++
    /**************************************************************/
	/* Comp 5: Compute Pipeline (10.1.)                           */
	/**************************************************************/
	std::vector<VkPipeline> compute_pipelines_arr(1);
	std::vector<VkComputePipelineCreateInfo> compute_pipelines_info_arr(1);
	compute_pipelines_info_arr[0].sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	compute_pipelines_info_arr[0].pNext = nullptr;
	compute_pipelines_info_arr[0].flags = 0;
	compute_pipelines_info_arr[0].stage = compute_shader_stage;
	compute_pipelines_info_arr[0].layout = compute_pipeline_layout;
	compute_pipelines_info_arr[0].basePipelineHandle = VK_NULL_HANDLE;
	compute_pipelines_info_arr[0].basePipelineHandle = 0;

	vk_result = vkCreateComputePipelines(
		logical_device,
		VK_NULL_HANDLE,
		compute_pipelines_info_arr.size(),
		compute_pipelines_info_arr.data(),
		nullptr,
		compute_pipelines_arr.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating compute pipeline!");
	}
```

File: [comp5_compute_pipeline.cpp](../../Code/comp_5_compute_pipeline.cpp)

| Previous | Next |
|---|---|
| [Comp 4.4: Descriptor Set Updates (14.2.4.)](comp4_4_descriptor_set_updates.md) | [Comp 6: Fences and Semaphores (7.3. and 7.4.)](comp6_fences_and_semaphores.md) |
