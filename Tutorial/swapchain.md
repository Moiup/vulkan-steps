# **Step 5.1: Swapchain (33.10.)**
## **Hints**
To create the Swapchain, we need to call [`vkCreateSwapchainKHR`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap33.html#vkCreateSwapchainKHR).

## **The Code**
We need to get the size in pixel of the framebuffer. To do so, we use [`glfwGetFramebufferSize`](https://www.glfw.org/docs/3.3/group__window.html#ga0e2637a4161afb283f5300c7f94785c9)

Do not forget to destroy the Swapchain at the end with [`vkDestroySwapchainKHR`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap33.html#vkDestroySwapchainKHR)

```C++
    /**************************************************************/
	/* Step 5: Swapchain and Image Views                          */
	/**************************************************************/
	/**************************************************************/
	/* Step 5.1: Swapchain (33.10.)                               */
	/**************************************************************/
    int32_t width_in_pixels, height_in_pixels;
	glfwGetFramebufferSize(window, &width_in_pixels, &height_in_pixels);

	VkSwapchainKHR swapchain{};
	VkSwapchainCreateInfoKHR swapchain_info{};
	swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.pNext = nullptr;
	swapchain_info.flags = 0;
	swapchain_info.surface = surface;
	swapchain_info.minImageCount = nb_frame;
	swapchain_info.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
	swapchain_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchain_info.imageExtent = { static_cast<uint32_t>(width_in_pixels), static_cast<uint32_t>(height_in_pixels) };
	swapchain_info.imageArrayLayers = 1;
	swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (queue_family_indexes.size() == 1) {
		swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	else {
		swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	}
	swapchain_info.queueFamilyIndexCount = queue_family_indexes.size();
	swapchain_info.pQueueFamilyIndices = queue_family_indexes.data();
	swapchain_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	swapchain_info.clipped = VK_TRUE;
	swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	vk_result = vkCreateSwapchainKHR(
		logical_device,
		&swapchain_info,
		nullptr,
		&swapchain
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to create swapchain!");
	}
	std::cout << "Swapchain created." << std::endl;
	}
```

File: [step_5_1_swapchain.cpp](../Code/step_5_1_swapchain.cpp)

| Previous | Next |
|---|---|
| [Step 5: Swapchain and Image Views](swapchain_and_image_views.md) | [Step 5.2: Image Views (12.5.)](image_views.md) |