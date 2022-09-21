# **Step 8.2: Pipeline Layouts (14.2.2.)**
## **Hints**
To create Pipeline Layout, we use the function [`vkCreatePipelineLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkCreatePipelineLayout).

## **The Code**
Fill the [`VkPipelineLayoutCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkPipelineLayoutCreateInfo) structure and call [`vkCreatePipelineLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkCreatePipelineLayout).

Destroy the Pipeline Layout by calling [`vkDestroyPipelineLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkDestroyPipelineLayout).

```C++
    /**************************************************************/
	/* Step 8.2: Pipeline Layouts (14.2.2.)                       */
	/**************************************************************/
	VkPipelineLayout pipeline_layout = nullptr;
	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.pNext = nullptr;
	pipeline_layout_info.flags = 0;
	pipeline_layout_info.setLayoutCount = descriptor_set_layout_arr.size();
	pipeline_layout_info.pSetLayouts = descriptor_set_layout_arr.data();
	pipeline_layout_info.pushConstantRangeCount = 0;
	pipeline_layout_info.pPushConstantRanges = nullptr;
	vk_result = vkCreatePipelineLayout(
		logical_device,
		&pipeline_layout_info,
		nullptr,
		&pipeline_layout
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the pipeline layout!");
	}
```

File: [step_8_2_pipeline_layouts.cpp](../Code/step_8_2_pipeline_layouts.cpp)

| Previous | Next |
|---|---|
| [Step 8.1: Descriptor Set Layout (14.2.1.)](descriptor_set_layout.md) | [Step 8.3: Allocation of Descriptor Sets (14.2.3.)](allocation_of_descriptor_sets.md) |