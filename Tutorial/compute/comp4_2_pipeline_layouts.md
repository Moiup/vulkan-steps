# **Comp 4.2:  Pipeline Layouts (14.2.2.)**
## **Hints**
To create Pipeline Layout, we use the function [`vkCreatePipelineLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkCreatePipelineLayout).

## **The Code**
Fill the [`VkPipelineLayoutCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkPipelineLayoutCreateInfo) structure and call [`vkCreatePipelineLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkCreatePipelineLayout).

Destroy the Pipeline Layout by calling [`vkDestroyPipelineLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkDestroyPipelineLayout).

```C++
    /**************************************************************/
	/* Comp 4.2: Pipeline Layouts (14.2.2.)                       */
	/**************************************************************/
	VkPipelineLayout compute_pipeline_layout = nullptr;
	VkPipelineLayoutCreateInfo compute_pipeline_layout_info{};
	compute_pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	compute_pipeline_layout_info.pNext = nullptr;
	compute_pipeline_layout_info.flags = 0;
	compute_pipeline_layout_info.setLayoutCount = compute_descriptor_set_layout_arr.size();
	compute_pipeline_layout_info.pSetLayouts = compute_descriptor_set_layout_arr.data();
	compute_pipeline_layout_info.pushConstantRangeCount = 0;
	compute_pipeline_layout_info.pPushConstantRanges = nullptr;
	vk_result = vkCreatePipelineLayout(
		logical_device,
		&compute_pipeline_layout_info,
		nullptr,
		&compute_pipeline_layout
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the compute pipeline layout!");
	}
```

File: [comp4_2_pipeline_layouts.cpp](../../Code/comp_4_2_pipeline_layouts.cpp)

| Previous | Next |
|---|---|
| [Comp 4.1: Descriptor Sets (14.2.)](comp4_1_descriptor_set_layout.md) | [Comp 4.3: Allocation of Descriptor Sets (14.2.4.)](comp4_3_allocation_of_descriptor_sets.md) |
