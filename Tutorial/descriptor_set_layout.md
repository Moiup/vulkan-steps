# **Step 8.1: Descriptor Set Layout (14.2.1.)**
## **Hints**
Descriptor Set Layout are created using [`vkCreateDescriptorSetLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkCreateDescriptorSetLayout).

## **The Code**
When creating Descriptor Set Layout, we are in fact creating an array of [`VkDescriptorSetLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#descriptorsets-setlayout), this will, in parallel, require an array of [`VkDescriptorSetLayoutBinding`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorSetLayoutBinding) to be affected inside the [`VkDescriptorSetLayoutCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorSetLayoutCreateInfo) structure. Then we can call [`vkCreateDescriptorSetLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkCreateDescriptorSetLayout).

We need to destroy each Descriptor Set Layout with [`vkDestroyDescriptorSetLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkDestroyDescriptorSetLayout).

```C++
    /**************************************************************/
	/* Step 8: Descriptor Sets (14.2.)                            */
	/**************************************************************/
	/**************************************************************/
	/* Step 8.1: Descriptor Set Layout (14.2.1.)                  */
	/**************************************************************/
	VkDescriptorSetLayoutBinding descriptor_set_binding;
	descriptor_set_binding.binding = 0;
	descriptor_set_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_set_binding.descriptorCount = 1;
	descriptor_set_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptor_set_binding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> descriptor_set_binding_arr = { descriptor_set_binding };

	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
	descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_info.pNext = nullptr;
	descriptor_set_layout_info.flags = 0;
	descriptor_set_layout_info.bindingCount = descriptor_set_binding_arr.size();
	descriptor_set_layout_info.pBindings = descriptor_set_binding_arr.data();

	std::vector<VkDescriptorSetLayout> descriptor_set_layout_arr(descriptor_set_binding_arr.size());
	vk_result = vkCreateDescriptorSetLayout(
		logical_device,
		&descriptor_set_layout_info,
		nullptr,
		descriptor_set_layout_arr.data()
	);
```

File: [step_8_1_descriptor_set_layout.cpp](../Code/step_8_1_descriptor_set_layout.cpp)

| Previous | Next |
|---|---|
| [Step 8: Descriptor Sets (14.2.)](descriptor_sets.md) | [Step 8.2: Pipeline Layouts (14.2.2.)](pipeline_layouts.md) |