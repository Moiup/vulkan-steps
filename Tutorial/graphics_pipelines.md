# **Step 13: Graphics Pipelines (10.2.)**
## **Hints**
To create the graphic pipeline: [`vkCreateGraphicsPipelines`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap10.html#vkCreateGraphicsPipelines).

## **The Code**


```C++
    /**************************************************************/
	/* Step 13: Graphics Pipelines (10.2.)                        */
	/**************************************************************/
	//---- Vertex Input State (22.2)----//
	VkVertexInputBindingDescription coord_buff_input_binding_desc{};
	coord_buff_input_binding_desc.binding = 0; // Cette valeur doit être vu comme un identifiant, c'est pour faire le lien ensuite avec la structure VkVertexInnputAttributeDescription (attention, il n'y a pas de lien avec les valeurs dans le shader)
	coord_buff_input_binding_desc.stride = sizeof(vert_coord[0]);
	coord_buff_input_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputBindingDescription color_buff_input_binding_desc{};
	color_buff_input_binding_desc.binding = 1;
	color_buff_input_binding_desc.stride = sizeof(vert_color[0]);
	color_buff_input_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputBindingDescription> vertex_input_binding_desc_arr = {
		coord_buff_input_binding_desc,
		color_buff_input_binding_desc
	};

	VkVertexInputAttributeDescription coord_input_desc{};
	coord_input_desc.location = 0;
	coord_input_desc.binding = coord_buff_input_binding_desc.binding;
	coord_input_desc.format = VK_FORMAT_R32G32B32_SFLOAT;
	coord_input_desc.offset = 0;

	VkVertexInputAttributeDescription color_input_desc{};
	color_input_desc.location = 1;
	color_input_desc.binding = color_buff_input_binding_desc.binding;
	color_input_desc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	color_input_desc.offset = 0;

	std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_arr{
		coord_input_desc,
		color_input_desc
	};

	VkPipelineVertexInputStateCreateInfo vertex_input_state_info{};
	vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state_info.pNext = nullptr;
	vertex_input_state_info.flags = 0;
	vertex_input_state_info.vertexBindingDescriptionCount = vertex_input_binding_desc_arr.size();
	vertex_input_state_info.pVertexBindingDescriptions = vertex_input_binding_desc_arr.data();
	vertex_input_state_info.vertexAttributeDescriptionCount = vertex_input_attribute_arr.size();
	vertex_input_state_info.pVertexAttributeDescriptions = vertex_input_attribute_arr.data();

	//---- Input Assembly State (22.2.)----//
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info{};
	input_assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_state_info.pNext = nullptr;
	input_assembly_state_info.flags = 0;
	input_assembly_state_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_state_info.primitiveRestartEnable = VK_FALSE;

	//---- Viewport State (26.9.)----//
	std::vector<VkViewport> viewport_arr;
	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)swapchain_info.imageExtent.width;
	viewport.height = (float)swapchain_info.imageExtent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1.0;

	viewport_arr.push_back(viewport);

	std::vector<VkRect2D> scissor_arr;
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchain_info.imageExtent;

	scissor_arr.push_back(scissor);

	VkPipelineViewportStateCreateInfo viewport_state_info{};
	viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_info.pNext = nullptr;
	viewport_state_info.flags = 0;
	viewport_state_info.viewportCount = viewport_arr.size();
	viewport_state_info.pViewports = viewport_arr.data();
	viewport_state_info.scissorCount = scissor_arr.size();
	viewport_state_info.pScissors = scissor_arr.data();

	//---- Rasterization State (27.)----//
	VkPipelineRasterizationStateCreateInfo rasterization_state_info{};
	rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_info.pNext = nullptr;
	rasterization_state_info.flags = 0;
	rasterization_state_info.depthClampEnable = VK_FALSE;
	rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_state_info.cullMode = VK_CULL_MODE_NONE;
	rasterization_state_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state_info.depthBiasEnable = VK_FALSE;
	rasterization_state_info.depthBiasConstantFactor = 0.0;
	rasterization_state_info.depthBiasClamp = 0.0;
	rasterization_state_info.depthBiasSlopeFactor = 0.0;
	rasterization_state_info.lineWidth = 1.0;

	//---- Multisample State ()----//
	VkPipelineMultisampleStateCreateInfo multisample_state_info{};
	multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_info.pNext = nullptr;
	multisample_state_info.flags = 0;
	multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_state_info.sampleShadingEnable = VK_FALSE;
	multisample_state_info.minSampleShading = 0;
	multisample_state_info.pSampleMask = nullptr;
	multisample_state_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_info.alphaToOneEnable = VK_FALSE;

	//---- Depth Stencil State ()----//
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state_info{};
	depth_stencil_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_state_info.pNext = nullptr;
	depth_stencil_state_info.flags = 0;
	depth_stencil_state_info.depthTestEnable = VK_TRUE;
	depth_stencil_state_info.depthWriteEnable = VK_TRUE;
	depth_stencil_state_info.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depth_stencil_state_info.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_state_info.stencilTestEnable = VK_FALSE;
	depth_stencil_state_info.front.failOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state_info.front.passOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state_info.front.compareOp = VK_COMPARE_OP_ALWAYS;
	depth_stencil_state_info.front.compareMask = 0;
	depth_stencil_state_info.front.reference = 0;
	depth_stencil_state_info.front.depthFailOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state_info.front.writeMask = 0;
	depth_stencil_state_info.back = depth_stencil_state_info.front;
	depth_stencil_state_info.minDepthBounds = 0;
	depth_stencil_state_info.maxDepthBounds = 0;

	//---- Color Blend State ()----//
	std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_arr;
	VkPipelineColorBlendAttachmentState color_blend_attachment{};
	color_blend_attachment.blendEnable = VK_FALSE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.colorWriteMask = 0xf;

	color_blend_attachment_arr.push_back(color_blend_attachment);

	VkPipelineColorBlendStateCreateInfo color_blend_state_info{};
	color_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_state_info.pNext = nullptr;
	color_blend_state_info.flags = 0;
	color_blend_state_info.logicOpEnable = VK_FALSE;
	color_blend_state_info.logicOp = VK_LOGIC_OP_CLEAR;
	color_blend_state_info.attachmentCount = color_blend_attachment_arr.size();
	color_blend_state_info.pAttachments = color_blend_attachment_arr.data();
	color_blend_state_info.blendConstants[0] = 1.0;
	color_blend_state_info.blendConstants[1] = 1.0;
	color_blend_state_info.blendConstants[2] = 1.0;
	color_blend_state_info.blendConstants[3] = 1.0;

	//---- Dynamic State (10.11.)----//
	std::vector<VkDynamicState> dynamic_state_arr = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamic_state_info{};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.pNext = nullptr;
	dynamic_state_info.flags = 0;
	dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(dynamic_state_arr.size());
	dynamic_state_info.pDynamicStates = dynamic_state_arr.data();

	//---- Creating the pipeline (10.2.)----//
	VkPipeline graphic_pipeline;
	VkGraphicsPipelineCreateInfo graphic_pipeline_info{};
	graphic_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphic_pipeline_info.pNext = nullptr;
	graphic_pipeline_info.flags = 0;
	graphic_pipeline_info.stageCount = shader_stage_arr.size();
	graphic_pipeline_info.pStages = shader_stage_arr.data();
	graphic_pipeline_info.pVertexInputState = &vertex_input_state_info;
	graphic_pipeline_info.pInputAssemblyState = &input_assembly_state_info;
	graphic_pipeline_info.pTessellationState = nullptr;
	graphic_pipeline_info.pViewportState = &viewport_state_info;
	graphic_pipeline_info.pRasterizationState = &rasterization_state_info;
	graphic_pipeline_info.pMultisampleState = &multisample_state_info;
	graphic_pipeline_info.pDepthStencilState = &depth_stencil_state_info;
	graphic_pipeline_info.pColorBlendState = &color_blend_state_info;
	graphic_pipeline_info.pDynamicState = &dynamic_state_info;
	graphic_pipeline_info.layout = pipeline_layout;
	graphic_pipeline_info.renderPass = render_pass;
	graphic_pipeline_info.subpass = 0;
	graphic_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	graphic_pipeline_info.basePipelineIndex = 0;

	vk_result = vkCreateGraphicsPipelines(
		logical_device,
		VK_NULL_HANDLE,
		1,
		&graphic_pipeline_info,
		nullptr,
		&graphic_pipeline
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to create graphic pipeline!");
	}
	else {
		std::cout << "Graphic pipeline created." << std::endl;
	}
```

File: [step_13_graphics_pipelines.cpp](../Code/step_13_graphics_pipelines.cpp)

| Previous | Next |
|---|---|
| [Step 12.2: Coordinate and Color Buffers (12.1 and 12.7)](coordinate_and_color_buffers.md) | [Step 14: Fences and Semaphores (7.3. and 7.4.)](fences_and_semaphores.md) |