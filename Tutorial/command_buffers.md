# **Step 4: Command Buffers (6.)**
## **Hints**
To create a command buffer : [`vkAllocateCommandBuffers`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#vkAllocateCommandBuffers)

## **The Code**
When wanting to create Command Buffers, the [`VkCommandBufferAllocateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#VkCommandBufferAllocateInfo) structure requires a command pool, created with [`vkCreateCommandPool`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#vkCreateCommandPool).

Do not forget to free the command buffers with [`vkFreeCommandBuffers`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#vkFreeCommandBuffers) and then destroy the command pool with [`vkDestroyCommandPool`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap6.html#vkDestroyCommandPool).

```C++
    /**************************************************************/
	/* Command Buffer (6.)                                        */
	/**************************************************************/
	/***********************/
	/* Command Pool (6.2.) */
	/***********************/
	VkCommandPool command_pool{};
	VkCommandPoolCreateInfo command_pool_create_info{};
	command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.pNext = nullptr;
	command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	command_pool_create_info.queueFamilyIndex = graphics_queues_arr[0].queueFamilyIndex;

	vk_result = vkCreateCommandPool(
		logical_device,
		&command_pool_create_info,
		nullptr,
		&command_pool
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: command pool creation failed.");
	}

	/*************************/
	/* Command buffer (6.3.) */
	/*************************/
	const uint32_t nb_frame = 2;
	std::vector<VkCommandBuffer> command_buffer_arr(nb_frame);
	VkCommandBufferAllocateInfo command_buffer_allocate_info{};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.pNext = nullptr;
	command_buffer_allocate_info.commandPool = command_pool;
	command_buffer_allocate_info.commandBufferCount = (uint32_t)command_buffer_arr.size();
	command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	vk_result = vkAllocateCommandBuffers(
		logical_device,
		&command_buffer_allocate_info,
		command_buffer_arr.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating command buffer.");
	}
	std::cout << "Command buffer created." << std::endl;
```

File: [step_4_command_buffers.cpp](../Code/step_4_command_buffers.cpp)

| Previous | Next |
|---|---|
| [Step 3.2: Logical Devices and Queues (5.2 and 5.3)](logical_devices_and_queues.md) | [tep 5: Swapchain (33.10.)](swapchain.md) |