# **Step 9: Render Pass (8.1.)**
## **Hints**
To create the Render Pass: [`vkCreateRenderPass`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap8.html#vkCreateRenderPass).

## **The Code**
To fill the [`VkRenderPassCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap8.html#VkRenderPassCreateInfo) structure, we need to create an array of [`VkAttachmentDescription`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap8.html#VkAttachmentDescription) as well as creating a Subpass. This Subpass is in fact two different arrays: one array of [`VkAttachmentReference`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap8.html#VkAttachmentReference) structure and one array of [`VkSubpassDescription`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap8.html#VkSubpassDescription) strucutre. The second structure require to create an array of [`VkAttachmentDescription`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap8.html#VkAttachmentDescription), in our case we will have two attachment descriptions: color attachment and depth attachment.

At the end, we must not forget to destroy the Render Pass by calling [`vkDestroyRenderPass`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap8.html#vkDestroyRenderPass).

```C++
    /**************************************************************/
	/* Step 9: Descriptor Set Updates (8.1.)                      */
	/**************************************************************/
	// Attachment description
	std::vector<VkAttachmentDescription> attachment_description_arr(2);
	// Color attachment
	attachment_description_arr[0].flags = 0;
	attachment_description_arr[0].format = swapchain_info.imageFormat;
	attachment_description_arr[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachment_description_arr[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_arr[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_description_arr[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_arr[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_arr[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_arr[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// Depth attachment
	attachment_description_arr[1].flags = 0;
	attachment_description_arr[1].format = depth_buffer_info.format;
	attachment_description_arr[1].samples = depth_buffer_info.samples;
	attachment_description_arr[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_arr[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_arr[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_arr[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_arr[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_arr[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Subpass
	// Color attachment
	std::vector<VkAttachmentReference> color_attachments_arr(1);
	color_attachments_arr[0].attachment = 0;
	color_attachments_arr[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	// Depth attachment
	std::vector<VkAttachmentReference> depth_attachments_arr(1);
	depth_attachments_arr[0].attachment = 1;
	depth_attachments_arr[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::vector<VkSubpassDescription> subpass_description_arr(1);
	subpass_description_arr[0].flags = 0;
	subpass_description_arr[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_description_arr[0].inputAttachmentCount = 0;
	subpass_description_arr[0].pInputAttachments = nullptr;
	subpass_description_arr[0].colorAttachmentCount = color_attachments_arr.size();
	subpass_description_arr[0].pColorAttachments = color_attachments_arr.data();
	subpass_description_arr[0].pResolveAttachments = nullptr;
	subpass_description_arr[0].pDepthStencilAttachment = depth_attachments_arr.data();
	subpass_description_arr[0].preserveAttachmentCount = 0;
	subpass_description_arr[0].pPreserveAttachments = nullptr;

	// Subpass dependency
	std::vector<VkSubpassDependency> subpass_dependency_arr(1);
	subpass_dependency_arr[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dependency_arr[0].dstSubpass = 0;
	subpass_dependency_arr[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency_arr[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency_arr[0].srcAccessMask = 0;
	subpass_dependency_arr[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpass_dependency_arr[0].dependencyFlags = 0;

	// Render Pass
	VkRenderPass render_pass = nullptr;
	VkRenderPassCreateInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.pNext = nullptr;
	render_pass_info.flags = 0;
	render_pass_info.attachmentCount = attachment_description_arr.size();
	render_pass_info.pAttachments = attachment_description_arr.data();
	render_pass_info.subpassCount = subpass_description_arr.size();
	render_pass_info.pSubpasses = subpass_description_arr.data();
	render_pass_info.dependencyCount = subpass_dependency_arr.size();
	render_pass_info.pDependencies = subpass_dependency_arr.data();

	vk_result = vkCreateRenderPass(
		logical_device,
		&render_pass_info,
		nullptr,
		&render_pass
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating render pass!");
	}
	std::cout << "Render pass created." << std::endl;
```

File: [step_9_render_pass.cpp](../Code/step_9_render_pass.cpp)

| Previous | Next |
|---|---|
| [Step 8.4: Descriptor Set Updates (14.2.4.)](descriptor_set_updates.md) | [Step 10: Shader](shader.md) |