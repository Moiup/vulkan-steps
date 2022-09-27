# **Step 8.3: Allocation of Descriptor Sets (14.2.3.)**
## **Hints**
To allocate memory for Descriptor : [`vkAllocateDescriptorSets`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkAllocateDescriptorSets). 

## **The Code**
When allocating memory for Descriptor Sets, we in fact give an array of [`VkDescriptorSet`]() to the function [`vkAllocateDescriptorSets`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkAllocateDescriptorSets). Thus, for each [`VkDescriptorSet`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorSet), we need to fill a [`VkDescriptorSetAllocateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorSetAllocateInfo) structure. Moreover, to properly fill this structure we must create a Descriptor Pool.

To create a Descriptor Pool, we call the [`vkCreateDescriptorPool`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkCreateDescriptorPool) function which require a [`VkDescriptorPoolCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorPoolCreateInfo) structure to which we must affect an array of [`VkDescriptorPoolSize`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorPoolSize) in the `pPoolSizes` field.

We must not forget to free each Descriptor Set, as well to destroy the created Descriptor Pool.
 - [`vkFreeDescriptorSets`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkFreeDescriptorSets) 
 - [`vkDestroyDescriptorPool`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkDestroyDescriptorPool)

```C++
    /**************************************************************/
	/* Step 8.3: Allocation of Descriptor Sets (14.2.3.)          */
	/**************************************************************/
	// descriptor pool
	VkDescriptorPoolSize descriptor_pool_size{};
	descriptor_pool_size.type = descriptor_set_binding_arr[0].descriptorType;
	descriptor_pool_size.descriptorCount = 1;

	std::vector<VkDescriptorPoolSize> descriptor_pool_size_arr{ descriptor_pool_size };

	VkDescriptorPool descriptor_pool = nullptr;
	VkDescriptorPoolCreateInfo descriptor_pool_info{};
	descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_info.pNext = nullptr;
	descriptor_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptor_pool_info.maxSets = 1;
	descriptor_pool_info.poolSizeCount = descriptor_pool_size_arr.size();
	descriptor_pool_info.pPoolSizes = descriptor_pool_size_arr.data();

	vk_result = vkCreateDescriptorPool(
		logical_device,
		&descriptor_pool_info,
		nullptr,
		&descriptor_pool
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the descriptor pool.");
	}

	// Allocating descriptor sets
	std::vector<VkDescriptorSet> descriptor_sets_arr(descriptor_pool_size_arr.size());
	std::vector<VkDescriptorSetAllocateInfo> descriptor_sets_info_arr(descriptor_pool_size_arr.size());
	descriptor_sets_info_arr[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_sets_info_arr[0].pNext = nullptr;
	descriptor_sets_info_arr[0].descriptorPool = descriptor_pool;
	descriptor_sets_info_arr[0].descriptorSetCount = descriptor_pool_size_arr.size();
	descriptor_sets_info_arr[0].pSetLayouts = descriptor_set_layout_arr.data();

	vk_result = vkAllocateDescriptorSets(
		logical_device,
		descriptor_sets_info_arr.data(),
		descriptor_sets_arr.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating descriptor sets.");
	}
```

File: [step_8_3_allocation_of_descriptor_sets.cpp](../Code/step_8_3_allocation_of_descriptor_sets.cpp)

| Previous | Next |
|---|---|
| [Step 8.2: Pipeline Layouts (14.2.2.)](pipeline_layouts.md) | [Step 8.4: Descriptor Set Updates (14.2.4.)](descriptor_set_updates.md) |
