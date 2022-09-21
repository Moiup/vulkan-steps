# **Step 8.4: Descriptor Set Updates (14.2.3.)**
## **Hints**
This is when we finaly affect the Uniform Buffer to the Descriptor Set. To do so, we must call the function [`vkUpdateDescriptorSets`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkUpdateDescriptorSets).

## **The Code**
Calling [`vkUpdateDescriptorSets`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkUpdateDescriptorSets) requires an array of [`VkWriteDescriptorSet`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkWriteDescriptorSet) which also requires an array of [`VkDescriptorBufferInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorBufferInfo).

```C++
    /**************************************************************/
	/* Step 8.4: Descriptor Set Updates (14.2.4.)                 */
	/**************************************************************/
	std::vector<VkDescriptorBufferInfo> descriptor_buffer_info(1);
	descriptor_buffer_info[0].buffer = uniform_buffer;
	descriptor_buffer_info[0].offset = 0;
	descriptor_buffer_info[0].range = VK_WHOLE_SIZE;

	std::vector<VkWriteDescriptorSet> descriptor_writes_arr(descriptor_sets_arr.size());
	descriptor_writes_arr[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_writes_arr[0].pNext = nullptr;
	descriptor_writes_arr[0].dstSet = descriptor_sets_arr[0];
	descriptor_writes_arr[0].dstBinding = descriptor_set_binding_arr[0].binding;
	descriptor_writes_arr[0].dstArrayElement = 0;
	descriptor_writes_arr[0].descriptorCount = 1;
	descriptor_writes_arr[0].descriptorType = descriptor_set_binding_arr[0].descriptorType;
	descriptor_writes_arr[0].pImageInfo = nullptr;
	descriptor_writes_arr[0].pBufferInfo = descriptor_buffer_info.data();
	descriptor_writes_arr[0].pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(
		logical_device,
		descriptor_writes_arr.size(),
		descriptor_writes_arr.data(),
		0,
		nullptr
	);
```

File: [step_8_4_descriptor_set_updates.cpp](../Code/step_8_4_descriptor_set_updates.cpp)

| Previous | Next |
|---|---|
| [Step 8.3: Allocation of Descriptor Sets (14.2.3.)](allocation_of_descriptor_sets.md) | [Step 9: Render Pass (8.1.)](render_pass.md) |