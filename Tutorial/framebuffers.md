# **Step 2.1: Physical Devices (5.1.)**
## **Hints**
To create a framebuffer: [`vkCreateFramebuffer`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap8.html#vkCreateFramebuffer)

## **The Code**
We need to create as many framebuffers as there are Image Views in the swapchain.

We must not forget to destroy all the framebuffers at the end with [`vkDestroyFramebuffer`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap8.html#vkDestroyFramebuffer)

```C++
    /**************************************************************/
	/* Step 11: Framebuffers (8.3.)                               */
	/**************************************************************/
	std::vector<VkFramebuffer> framebuffer_arr;

	for (size_t i = 0; i < swapchain_image_view_arr.size(); i++) {
		std::vector<VkImageView> attachments;

		attachments.push_back(swapchain_image_view_arr[i]);
		attachments.push_back(depth_image_view);

		VkFramebuffer framebuffer = nullptr;
		VkFramebufferCreateInfo framebuffer_info{};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.pNext = nullptr;
		framebuffer_info.flags = 0;
		framebuffer_info.renderPass = render_pass;
		framebuffer_info.attachmentCount = attachments.size();
		framebuffer_info.pAttachments = attachments.data();
		framebuffer_info.width = swapchain_info.imageExtent.width;
		framebuffer_info.height = swapchain_info.imageExtent.height;
		framebuffer_info.layers = 1;

		vk_result = vkCreateFramebuffer(
			logical_device,
			&framebuffer_info,
			nullptr,
			&framebuffer
		);

		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create framebuffer!");
		}

		framebuffer_arr.push_back(framebuffer);
	}
	std::cout << "Framebuffer created." << std::endl;
```

File: [step_11_framebuffers.cpp](../Code/step_11_framebuffers.cpp)

| Previous | Next |
|---|---|
| [Step 10.2: Shader Modules (9.1)](shader_modules.md) | [Step 12: Vertex Buffers (12.1. and 12.7.)](vertex_buffers.md) |