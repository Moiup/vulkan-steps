# **Step 5.2:  (12.5.)**
## **Hints**
To create an Image View : [`vkCreateImageView`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkCreateImageView).

## **The Code**
You must first get number of image in the swapchain by using [`vkGetSwapchainImagesKHR`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap33.html#vkGetSwapchainImagesKHR). Then, you can create the [VkImageViews](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#VkImageView), and for each of the associated [`VkImageViewCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#VkImageViewCreateInfo) we need to associate the [`VkImage`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#resources-images) in the field `image`.

Do not forget to destroy all the [`VkImage`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#resources-images) with [`vkDestroyImageView`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap12.html#vkDestroyImageView)

```C++
    /**************************************************************/
	/* Step 5.2: Image Views                                      */
	/**************************************************************/
	uint32_t swapchain_image_count;
	std::vector<VkImage> swapchain_image;
	vk_result = vkGetSwapchainImagesKHR(
		logical_device,
		swapchain,
		&swapchain_image_count,
		nullptr
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to access swapchain image count!");
	}
	std::cout << "image_count: " << swapchain_image_count << std::endl;

	swapchain_image.resize(swapchain_image_count);
	vk_result = vkGetSwapchainImagesKHR(
		logical_device,
		swapchain,
		&swapchain_image_count,
		swapchain_image.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to retrieve the swapchain images!");
	}

	std::vector<VkImageView> swapchain_image_view_arr(swapchain_image_count);
	for (size_t i = 0; i < swapchain_image_count; i++) {
		VkImageViewCreateInfo  swapchain_image_view_info{};
		swapchain_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		swapchain_image_view_info.pNext = nullptr;
		swapchain_image_view_info.flags = 0;
		swapchain_image_view_info.image = swapchain_image[i];
		swapchain_image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		swapchain_image_view_info.format = swapchain_info.imageFormat;
		swapchain_image_view_info.components.r = VK_COMPONENT_SWIZZLE_R;
		swapchain_image_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
		swapchain_image_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
		swapchain_image_view_info.components.a = VK_COMPONENT_SWIZZLE_A;
		swapchain_image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		swapchain_image_view_info.subresourceRange.baseMipLevel = 0;
		swapchain_image_view_info.subresourceRange.levelCount = 1;
		swapchain_image_view_info.subresourceRange.baseArrayLayer = 0;
		swapchain_image_view_info.subresourceRange.layerCount = 1;

		vk_result = vkCreateImageView(
			logical_device,
			&swapchain_image_view_info,
			nullptr,
			&swapchain_image_view_arr[i]
		);
	}
```

File: [step_5_2_image_views.cpp](../Code/step_5_2_image_views.cpp)

| Previous | Next |
|---|---|
| [Step 5.1: Swapchain (33.10.)](swapchain.md) | [Step 6: Depth Buffer (12.3., 12.5. and 12.7.)](depth_buffer.md) |