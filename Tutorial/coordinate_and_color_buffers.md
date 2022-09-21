# **Step 12.2: Coordinate and Color Buffers (12.1. and 12.7.)**
## **Hints**
The steps are the same as making an [Uniform Buffer](uniform_buffer.md)

Reminder:
To make a Vertex Buffer:
 - We need to make an entity that will be stored in a Vertex Buffer such as the coordinate of the cube.
 - ...
 - Bind the Uniform Buffer: [`vkBindBufferMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkBindBufferMemory).

## **The Code**
To make a Vertex Buffer (complete version):
 -We need to make an entity that will be stored in a Vertex Buffer such as the coordinate of the cube.
 - Create the Vertex Buffer with [`vkCreateBuffer`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkCreateBuffer).
 - Retreive the buffer memory requirements: [`vkGetBufferMemoryRequirements`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkGetBufferMemoryRequirements).
 - Allocate the memory: [`vkAllocateMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkAllocateMemory).
 - Associate the data with the Vertex Buffer: [`vkMapMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkMapMemory).

This time, we do not need to call `vkGetPhysicalDeviceMemoryProperties` again, we did it when allocating the memory for the Depth Buffer.

Do not forget to free the memory as well as destroying the buffer:
 - [`vkFreeMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkFreeMemory)
 - [`vkDestroyBuffer`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkDestroyBuffer)

 These steps have to be done twice as we make two vertex buffers.

- Coordinate Buffer:
```C++
    /**************************************************************/
	/* Step 12.2: Coordinate and Color Buffers (12.1. and 12.7.)  */
	/**************************************************************/
	// Creating the COORD BUFFER
	VkBuffer coord_buffer = nullptr;
	VkBufferCreateInfo coord_buffer_info{};
	coord_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	coord_buffer_info.pNext = nullptr;
	coord_buffer_info.flags = 0;
	coord_buffer_info.size = vert_coord.size() * sizeof(vert_coord[0]);
	coord_buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	coord_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	coord_buffer_info.queueFamilyIndexCount = 0;
	coord_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		logical_device,
		&coord_buffer_info,
		nullptr,
		&coord_buffer
	);
	if (vk_result != VK_SUCCESS) {
		std::cout << "Error: failed creating vertex buffer!" << std::endl;
	}

	// Allocating 
	VkMemoryRequirements coord_buffer_mem_req{};
	vkGetBufferMemoryRequirements(
		logical_device,
		coord_buffer,
		&coord_buffer_mem_req
	);

	VkDeviceMemory coord_buffer_memory = nullptr;
	VkMemoryAllocateInfo coord_buffer_memory_info{};
	coord_buffer_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	coord_buffer_memory_info.pNext = nullptr;
	coord_buffer_memory_info.allocationSize = coord_buffer_mem_req.size;
	coord_buffer_memory_info.memoryTypeIndex = 0;

	memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((coord_buffer_mem_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			coord_buffer_memory_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: vertex buffer memory type not found!");
	}

	vk_result = vkAllocateMemory(
		logical_device,
		&coord_buffer_memory_info,
		nullptr,
		&coord_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		std::cout << "Error: failed allocating vertex buffer memory!" << std::endl;
	}
	std::cout << "coord_buffer_mem_req.size: " << coord_buffer_mem_req.size << std::endl;
	// Maping value
	float* coord_copy = new float[vert_coord.size() * vert_coord[0].length()];
	vk_result = vkMapMemory(
		logical_device,
		coord_buffer_memory,
		0,
		coord_buffer_mem_req.size,
		0,
		(void**)&coord_copy
	);
	memcpy(coord_copy, vert_coord.data(), coord_buffer_info.size);
	vkUnmapMemory(logical_device, coord_buffer_memory);

	// Binding
	vk_result = vkBindBufferMemory(
		logical_device,
		coord_buffer,
		coord_buffer_memory,
		0
	);
	if (vk_result != VK_SUCCESS) {
		std::cout << "Error: failed binding the vertex buffer memory!" << std::endl;
	}
	std::cout << "Coord buffer created." << std::endl;
```

- Color Buffer:
```C++
	// Creating the	COLOR BUFFER
	VkBuffer color_buffer = nullptr;
	VkBufferCreateInfo color_buffer_info{};
	color_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	color_buffer_info.pNext = nullptr;
	color_buffer_info.flags = 0;
	color_buffer_info.size = vert_color.size() * sizeof(vert_color[0]);
	color_buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	color_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	color_buffer_info.queueFamilyIndexCount = 0;
	color_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		logical_device,
		&color_buffer_info,
		nullptr,
		&color_buffer
	);

	// Allocating memory
	VkMemoryRequirements color_buffer_mem_req{};
	vkGetBufferMemoryRequirements(
		logical_device,
		color_buffer,
		&color_buffer_mem_req
	);

	VkMemoryAllocateInfo color_buffer_memory_info{};
	color_buffer_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	color_buffer_memory_info.pNext = nullptr;
	color_buffer_memory_info.allocationSize = color_buffer_mem_req.size;
	color_buffer_memory_info.memoryTypeIndex = 0;

	memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((coord_buffer_mem_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			color_buffer_memory_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: color buffer memory type not found!");
	}

	VkDeviceMemory color_buffer_memory = nullptr;
	vk_result = vkAllocateMemory(
		logical_device,
		&color_buffer_memory_info,
		nullptr,
		&color_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating color buffer memory!");
	}

	// Mapping value
	float* color_copy = new float[vert_color.size() * vert_color[0].length()];
	vk_result = vkMapMemory(
		logical_device,
		color_buffer_memory,
		0,
		color_buffer_mem_req.size,
		0,
		(void**)&color_copy
	);
	memcpy(color_copy, vert_color.data(), color_buffer_info.size);
	vkUnmapMemory(logical_device, color_buffer_memory);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed mapping color buffer values.");
	}

	// Binding
	vk_result = vkBindBufferMemory(
		logical_device,
		color_buffer,
		color_buffer_memory,
		0
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed binding color buffer values.");
	}
	std::cout << "Color buffer created." << std::endl;
```

File: [step_12_2_coordinate_and_color_buffers.cpp](../Code/step_12_2_coordinate_and_color_buffers.cpp)

| Previous | Next |
|---|---|
| [Step 12.1: Data](data.md) | [Step 13: Graphics Pipelines (10.2.)](graphics_pipelines.md) |