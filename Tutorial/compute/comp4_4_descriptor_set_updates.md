# **Comp 4.3: Descriptor Set Updates (14.2.4.)**
## **Hints**
This is when we finaly affect the Uniform Buffer to the Descriptor Set. To do so, we must call the function [`vkUpdateDescriptorSets`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkUpdateDescriptorSets).

## **The Code**
Calling [`vkUpdateDescriptorSets`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkUpdateDescriptorSets) requires an array of [`VkWriteDescriptorSet`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkWriteDescriptorSet) which also requires an array of [`VkDescriptorBufferInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorBufferInfo).

```C++
    /**************************************************************/
	/* Comp 4.4: Allocation of Descriptor Sets (14.2.4.)          */
	/**************************************************************/
	std::vector<VkDescriptorBufferInfo> compute_descriptor_buffer_info(compute_descriptor_set_binding_arr.size());

	compute_descriptor_buffer_info[0].buffer = input_data_buffer;
	compute_descriptor_buffer_info[0].offset = 0;
	compute_descriptor_buffer_info[0].range = VK_WHOLE_SIZE;

	compute_descriptor_buffer_info[1].buffer = output_data_buffer;
	compute_descriptor_buffer_info[1].offset = 0;
	compute_descriptor_buffer_info[1].range = VK_WHOLE_SIZE;

	std::vector<VkWriteDescriptorSet> compute_descriptor_writes_arr(1);
	compute_descriptor_writes_arr[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	compute_descriptor_writes_arr[0].pNext = nullptr;
	compute_descriptor_writes_arr[0].dstSet = compute_descriptor_sets_arr[0];
	compute_descriptor_writes_arr[0].dstBinding = 0;
	compute_descriptor_writes_arr[0].dstArrayElement = 0;
	compute_descriptor_writes_arr[0].descriptorCount = compute_descriptor_buffer_info.size();
	compute_descriptor_writes_arr[0].descriptorType = compute_descriptor_set_binding_arr[0].descriptorType;
	compute_descriptor_writes_arr[0].pImageInfo = nullptr;
	compute_descriptor_writes_arr[0].pBufferInfo = compute_descriptor_buffer_info.data();
	compute_descriptor_writes_arr[0].pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(
		logical_device,
		compute_descriptor_writes_arr.size(),
		compute_descriptor_writes_arr.data(),
		0,
		nullptr
	);
```

File: [comp4_4_descriptor_set_updates.cpp](../../Code/comp_4_4_descriptor_set_updates.cpp)

| Previous | Next |
|---|---|
| [Comp 4.3: Allocation of Descriptor Sets (14.2.3.)](comp4_3_allocation_of_descriptor_sets.md) | [Comp 5: Compute Pipeline (10.1.)](comp5_compute_pipeline.md) |
