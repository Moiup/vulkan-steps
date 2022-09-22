# **Step 14.1: Fences (7.3.)**
## **Hints**
To create a fence: [`vkCreateFence`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#vkCreateFence).

## **The Code**
We must create as many fences as there are framebuffers.

At the end, we must destroy each fence with [`vkDestroyFence`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#vkDestroyFence)

```C++
    /**************************************************************/
	/* Step 14: Fences and Semaphores (7.3. and 7.4.)             */
	/**************************************************************/
	/**************************************************************/
	/* Step 14.1: Fences (7.3.)                                   */
	/**************************************************************/
	std::vector<VkFence> in_flight_fence_arr(framebuffer_arr.size());
	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t fence_i = 0; fence_i < in_flight_fence_arr.size(); fence_i++) {
		vk_result = vkCreateFence(
			logical_device,
			&fence_info,
			nullptr,
			&in_flight_fence_arr[fence_i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating fence!");
		}
		else {
			std::cout << "Fence created." << std::endl;
		}
	}
```

File: [step_14_1_fences.cpp](../Code/step_14_1_fences.cpp)

| Previous | Next |
|---|---|
| [Step 14: Fences and Semaphores (7.3. and 7.4.)](fences_and_semaphores.md) | [Step 14.2: Semaphores (7.4)](semaphores.md) |