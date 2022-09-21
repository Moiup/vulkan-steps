# **Step 6: Depth Buffer (12.3., 12.5. and 12.7.)**
## **Hints**
Creating a Depth Buffer comes to creating an image view. To do saw, the function to call is [`vkCreateImageView`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkCreateImageView).

## **The Code**
When using [`vkCreateImageView`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkCreateImageView) we need to fill a [`VkImageViewCreateInfo`] which requires to create a [`VkImage`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#resources-images) by calling [`vkCreateImage`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkCreateImage).

After creating the image, we need to allocate memory. To do so, we first get the memory requirements with [`vkGetImageMemoryRequirements`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkGetImageMemoryRequirements), then the memory properties of the physical device with [`vkGetPhysicalDeviceMemoryProperties`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkGetPhysicalDeviceMemoryProperties). We need to iterate though the `memoryTypes` field of the [`VkPhysicalDeviceMemoryProperties`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#VkPhysicalDeviceMemoryProperties) to find the one having the `propertyFlags` that we are looking for. We can finally allocate the memory with [`vkAllocateMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap11.html#vkAllocateMemory).

Now, we bind the image memory thanks to [`vkBindImageMemory`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkBindImageMemory) and create the image view thanks to [`vkCreateImageView`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkCreateImageView).

```C++

    /**************************************************************/
	/* Step 6: Depth Buffer (12.3., 12.5. and 12.7.)              */
	/**************************************************************/
	// Creating the image storing the depth buffer
	VkImage depth_buffer = nullptr;
	VkImageCreateInfo depth_buffer_info{};
	depth_buffer_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	depth_buffer_info.pNext = nullptr;
	depth_buffer_info.flags = 0;
	depth_buffer_info.imageType = VK_IMAGE_TYPE_2D;
	depth_buffer_info.format = VK_FORMAT_D16_UNORM;
	depth_buffer_info.extent = { static_cast<uint32_t>(width_in_pixels), static_cast<uint32_t>(height_in_pixels), 1 };
	depth_buffer_info.mipLevels = 1;
	depth_buffer_info.arrayLayers = 1;
	depth_buffer_info.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_buffer_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	depth_buffer_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depth_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	depth_buffer_info.queueFamilyIndexCount = queue_family_indexes.size();
	depth_buffer_info.pQueueFamilyIndices = queue_family_indexes.data();
	depth_buffer_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	vk_result = vkCreateImage(
		logical_device,
		&depth_buffer_info,
		nullptr,
		&depth_buffer
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the depth buffer!");
	}

	// Getting memory requirements
	VkMemoryRequirements depth_buffer_memory_req{};
	vkGetImageMemoryRequirements(
		logical_device,
		depth_buffer,
		&depth_buffer_memory_req
	);

	// Allocating the memory
	VkDeviceMemory depth_buffer_memory;
	VkMemoryAllocateInfo depth_buffer_image_memory_allocate_info{};
	depth_buffer_image_memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	depth_buffer_image_memory_allocate_info.pNext = nullptr;
	depth_buffer_image_memory_allocate_info.allocationSize = depth_buffer_memory_req.size;
	depth_buffer_image_memory_allocate_info.memoryTypeIndex = -1;

	VkPhysicalDeviceMemoryProperties memory_properties{};
	vkGetPhysicalDeviceMemoryProperties(discrete_gpu, &memory_properties);
	std::cout << "Memory type count: " << memory_properties.memoryTypeCount << std::endl;
	uint32_t memory_type_i = 0;
	VkMemoryPropertyFlags memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((depth_buffer_memory_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			depth_buffer_image_memory_allocate_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: depth buffer memory type not found!");
	}

	vk_result = vkAllocateMemory(
		logical_device,
		&depth_buffer_image_memory_allocate_info,
		nullptr,
		&depth_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("error: failed allocating depth buffer memory!");
	}

	// Binding the memory
	vkBindImageMemory(
		logical_device,
		depth_buffer,
		depth_buffer_memory,
		0
	);

	// Creating the image view
	VkImageView depth_image_view{};
	VkImageViewCreateInfo depth_image_view_info{};
	depth_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depth_image_view_info.pNext = nullptr;
	depth_image_view_info.flags = 0;
	depth_image_view_info.image = depth_buffer;
	depth_image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depth_image_view_info.format = VK_FORMAT_D16_UNORM;
	depth_image_view_info.components.r = VK_COMPONENT_SWIZZLE_R;
	depth_image_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
	depth_image_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
	depth_image_view_info.components.a = VK_COMPONENT_SWIZZLE_A;
	depth_image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	depth_image_view_info.subresourceRange.baseMipLevel = 0;
	depth_image_view_info.subresourceRange.levelCount = 1;
	depth_image_view_info.subresourceRange.baseArrayLayer = 0;
	depth_image_view_info.subresourceRange.layerCount = 1;

	vkCreateImageView(
		logical_device,
		&depth_image_view_info,
		nullptr,
		&depth_image_view
	);
	std::cout << "Depth buffer created." << std::endl;
```

File: [step_6_depth_buffer.cpp](../Code/step_6_depth_buffer.cpp)

| Previous | Next |
|---|---|
| [Step 5.2: Image Views (12.5.)](image_views.md) | [Step 7: Uniform Buffer (12.1. and 12.7.)](uniform_buffer.md) |