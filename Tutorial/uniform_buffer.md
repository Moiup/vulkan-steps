# **Step 7: Uniform Buffer (12.1. and 12.7.)**
## **Hints**

To make a Uniform Buffer:
 - We need to make an entity that will be stored in a Uniform Buffer, for example an MVP matrix.
 - ...
 - Bind the Uniform Buffer: [`vkBindBufferMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkBindBufferMemory).


## **The Code**

To make a Uniform Buffer (complete version):
 - We need to make an entity that will be stored in a Uniform Buffer, for example an MVP matrix.
 - Create the Uniform Buffer with [`vkCreateBuffer`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkCreateBuffer).
 - Retreive the buffer memory requirements: [`vkGetBufferMemoryRequirements`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkGetBufferMemoryRequirements).
 - Allocate the memory: [`vkAllocateMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkAllocateMemory).
 - Associate the data with the Uniform Buffer: [`vkMapMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkMapMemory).

This time, we do not need to call `vkGetPhysicalDeviceMemoryProperties` again, we did it when allocating the memory for the Depth Buffer.

Do not forget to free the memory as well as destroying the buffer:
 - [`vkFreeMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkFreeMemory)
 - [`vkDestroyBuffer`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkDestroyBuffer)

```C++
    /**************************************************************/
	/* Step 7: Depth Buffer (12.1., and 12.7.)                    */
	/**************************************************************/
	// Setting the MVP Matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(-5, 3, -10),
		glm::vec3(0, 0, 0),
		glm::vec3(0, -1, 0)
	);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 clip = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f, 1.0f
	);

	glm::mat4 mvp = clip * projection * view * model;

	// Creating the uniform buffer
	VkBuffer uniform_buffer = nullptr;
	VkBufferCreateInfo uniform_buffer_info{};
	uniform_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	uniform_buffer_info.pNext = nullptr;
	uniform_buffer_info.flags = 0;
	uniform_buffer_info.size = sizeof(mvp);
	uniform_buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	uniform_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	uniform_buffer_info.queueFamilyIndexCount = 0;
	uniform_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		logical_device,
		&uniform_buffer_info,
		nullptr,
		&uniform_buffer
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating uniform buffer!");
	}

	// Requiring the memory information
	VkMemoryRequirements uniform_buffer_memory_req{};
	vkGetBufferMemoryRequirements(
		logical_device,
		uniform_buffer,
		&uniform_buffer_memory_req
	);

	// Allocating the memory
	VkDeviceMemory uniform_buffer_memory = nullptr;
	VkMemoryAllocateInfo uniform_buffer_memory_info{};
	uniform_buffer_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	uniform_buffer_memory_info.pNext = nullptr;
	uniform_buffer_memory_info.allocationSize = uniform_buffer_memory_req.size;
	uniform_buffer_memory_info.memoryTypeIndex = 0;

	// No need to call vkGetPhysicalDeviceMemoryProperties again, we did it when allocating the memory for the depth buffer
	memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((uniform_buffer_memory_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			uniform_buffer_memory_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: uniform buffer memory type not found!");
	}

	vk_result = vkAllocateMemory(
		logical_device,
		&uniform_buffer_memory_info,
		nullptr,
		&uniform_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating uniform buffer memory!");
	}

	// Mapping the memory
	float* mvp_copy = new float[4 * 4];
	vk_result = vkMapMemory(
		logical_device,
		uniform_buffer_memory,
		0,
		uniform_buffer_memory_req.size,
		0,
		(void**)&mvp_copy
	);
	memcpy(mvp_copy, &mvp, 4 * 4 * sizeof(float));
	vkUnmapMemory(logical_device, uniform_buffer_memory);

	// Binding the memory
	vk_result = vkBindBufferMemory(
		logical_device,
		uniform_buffer,
		uniform_buffer_memory,
		0
	);
	std::cout << "Uniform buffer created." << std::endl;
```

File: [step_7_uniform_buffer.cpp)](../Code/step_7_uniform_buffer.cpp)

| Previous | Next |
|---|---|
| [Step 6: Depth Buffer (12.3., 12.5. and 12.7.)](depth_buffer.md) | [Step 8: Descriptor Sets (14.2.)](descriptor_sets.md) |