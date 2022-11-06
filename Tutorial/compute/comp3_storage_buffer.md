# **Comp 3: Storage Buffer (12.1. and 12.7.)**
## **Hints**

The steps are the same as making a Uniform Buffer (see [Step 7: Uniform Buffer (12.1. and 12.7.)](../uniform_buffer.md)).

To make a Storage Buffer:
 - We need to make an entity that will be stored in a Storage Buffer, for example an array of integers.
 - ...
 - Bind the Storage Buffer: [`vkBindBufferMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkBindBufferMemory).


## **The Code**

To make a Storage Buffer (complete version):
 - We need to make an entity that will be stored in a Storage Buffer, for example an array of integers.
 - Create the Storage Buffer with [`vkCreateBuffer`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkCreateBuffer).
 - Retreive the buffer memory requirements: [`vkGetBufferMemoryRequirements`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkGetBufferMemoryRequirements).
 - Allocate the memory: [`vkAllocateMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkAllocateMemory).
 - Associate the data with the Storage Buffer: [`vkMapMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkMapMemory).

This time, we do not need to call `vkGetPhysicalDeviceMemoryProperties` again, we did it when allocating the memory for the Depth Buffer (see [Step 6: Depth Buffer (12.3., 12.5. and 12.7.)](../depth_buffer.md)).

Do not forget to free the memory as well as destroying the buffer:
 - [`vkFreeMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkFreeMemory)
 - [`vkDestroyBuffer`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkDestroyBuffer)


```C++
    /**************************************************************/
	/* Comp 3: Storage Buffer                                     */
	/**************************************************************/
	// --- data --- //
	std::vector<int> input_data(4, 2);
	std::vector<int> output_data(input_data.size(), -1);

	// --- Storage Buffer --- //
	// -- INPUT DATA -- //
	VkBuffer input_data_buffer = nullptr;
	VkBufferCreateInfo input_data_buffer_info{};
	input_data_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	input_data_buffer_info.pNext = nullptr;
	input_data_buffer_info.flags = 0;
	input_data_buffer_info.size = input_data.size() * sizeof(input_data[0]);
	input_data_buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	input_data_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	input_data_buffer_info.queueFamilyIndexCount = 0;
	input_data_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		logical_device,
		&input_data_buffer_info,
		nullptr,
		&input_data_buffer
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating storage buffer.");
	}

	VkMemoryRequirements input_data_buffer_memory_req{};
	vkGetBufferMemoryRequirements(
		logical_device,
		input_data_buffer,
		&input_data_buffer_memory_req
	);

	VkDeviceMemory input_data_buffer_memory = nullptr;
	VkMemoryAllocateInfo input_data_buffer_memory_info{};
	input_data_buffer_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	input_data_buffer_memory_info.pNext = nullptr;
	input_data_buffer_memory_info.allocationSize = input_data_buffer_memory_req.size;
	input_data_buffer_memory_info.memoryTypeIndex = 0;

	memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((input_data_buffer_memory_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			input_data_buffer_memory_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: uniform buffer memory type not found!");
	}

	vk_result = vkAllocateMemory(
		logical_device,
		&input_data_buffer_memory_info,
		nullptr,
		&input_data_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating input data storage buffer memory!");
	}

	int* input_data_copy;
	vk_result = vkMapMemory(
		logical_device,
		input_data_buffer_memory,
		0,
		input_data_buffer_memory_req.size,
		0,
		(void**)&input_data_copy
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: maping input data buffer memory failed!");
	}

	memcpy(input_data_copy, input_data.data(), input_data_buffer_memory_req.size);
	vkUnmapMemory(logical_device, input_data_buffer_memory);
	vk_result = vkBindBufferMemory(
		logical_device,
		input_data_buffer,
		input_data_buffer_memory,
		0
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: input data buffer binding failed!");
	}

	std::cout << "Input data buffer created." << std::endl;

	// -- OUTPUT DATA -- //
	VkBuffer output_data_buffer = nullptr;
	VkBufferCreateInfo output_data_buffer_info{};
	output_data_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	output_data_buffer_info.pNext = nullptr;
	output_data_buffer_info.flags = 0;
	output_data_buffer_info.size = output_data.size() * sizeof(output_data[0]);
	output_data_buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	output_data_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	output_data_buffer_info.queueFamilyIndexCount = 0;
	output_data_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		logical_device,
		&output_data_buffer_info,
		nullptr,
		&output_data_buffer
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating storage buffer.");
	}

	VkMemoryRequirements output_data_buffer_memory_req{};
	vkGetBufferMemoryRequirements(
		logical_device,
		output_data_buffer,
		&output_data_buffer_memory_req
	);

	VkDeviceMemory output_data_buffer_memory = nullptr;
	VkMemoryAllocateInfo output_data_buffer_memory_info{};
	output_data_buffer_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	output_data_buffer_memory_info.pNext = nullptr;
	output_data_buffer_memory_info.allocationSize = output_data_buffer_memory_req.size;
	output_data_buffer_memory_info.memoryTypeIndex = 0;

	memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((output_data_buffer_memory_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			output_data_buffer_memory_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: uniform buffer memory type not found!");
	}

	vk_result = vkAllocateMemory(
		logical_device,
		&output_data_buffer_memory_info,
		nullptr,
		&output_data_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating output data storage buffer memory!");
	}

	int* output_data_copy;
	vk_result = vkMapMemory(
		logical_device,
		output_data_buffer_memory,
		0,
		output_data_buffer_memory_req.size,
		0,
		(void**)&output_data_copy
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: maping output data buffer memory failed!");
	}

	memcpy(output_data_copy, output_data.data(), output_data_buffer_memory_req.size);
	vkUnmapMemory(logical_device, output_data_buffer_memory);
	vk_result = vkBindBufferMemory(
		logical_device,
		output_data_buffer,
		output_data_buffer_memory,
		0
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: output data buffer binding failed!");
	}

	std::cout << "Output data buffer created." << std::endl;
```

File: [comp3_storage_buffer.cpp](../../Code/comp3_storage_buffer.cpp)

| Previous | Next |
|---|---|
| [Comp 2.2: Shader Module (9.1)](comp2_2_shader_module.md) | [Comp 4: Descriptor Sets](comp4_descriptor_sets.md) |