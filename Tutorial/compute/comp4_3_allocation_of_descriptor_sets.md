# **Comp 4.3: Allocation of Descriptor Sets (14.2.3.)**
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
	/* Comp 4.3: Allocation of Descriptor Sets (14.2.3.)          */
	/**************************************************************/
	// ---- Descriptor Pool ---- //
	std::vector<VkDescriptorPoolSize> compute_descriptor_pool_size_arr(1);
	compute_descriptor_pool_size_arr[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	compute_descriptor_pool_size_arr[0].descriptorCount = 2;

	VkDescriptorPool compute_descriptor_pool = nullptr;
	VkDescriptorPoolCreateInfo compute_descriptor_pool_info{};
	compute_descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	compute_descriptor_pool_info.pNext = nullptr;
	compute_descriptor_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	compute_descriptor_pool_info.maxSets = compute_descriptor_set_binding_arr.size();
	compute_descriptor_pool_info.poolSizeCount = compute_descriptor_pool_size_arr.size();
	compute_descriptor_pool_info.pPoolSizes = compute_descriptor_pool_size_arr.data();

	vk_result = vkCreateDescriptorPool(
		logical_device,
		&compute_descriptor_pool_info,
		nullptr,
		&compute_descriptor_pool
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the compute descriptor pool!");
	}

	// Allocating compute descriptor sets
	std::vector<VkDescriptorSet> compute_descriptor_sets_arr(1);
	VkDescriptorSetAllocateInfo compute_descriptor_sets_info{};
	compute_descriptor_sets_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	compute_descriptor_sets_info.pNext = nullptr;
	compute_descriptor_sets_info.descriptorPool = compute_descriptor_pool;
	compute_descriptor_sets_info.descriptorSetCount = compute_descriptor_set_layout_arr.size();
	compute_descriptor_sets_info.pSetLayouts = compute_descriptor_set_layout_arr.data();

	vk_result = vkAllocateDescriptorSets(
		logical_device,
		&compute_descriptor_sets_info,
		compute_descriptor_sets_arr.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating compute descriptor sets!");
	}
```

File: [comp4_3_allocation_of_descriptor_sets.cpp](../../Code/comp_4_3_allocation_of_descriptor_sets.cpp)

| Previous | Next |
|---|---|
| [Comp 4.2: Pipeline Layouts (14.2.2.)](comp4_2_pipeline_layouts.md) | [Comp 4.4: Descriptor Set Updates (14.2.4.)](comp4_4_descriptor_set_updates.md) |
