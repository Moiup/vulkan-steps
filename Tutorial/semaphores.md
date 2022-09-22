# **Step 14.2: Semaphores (7.4.)**
## **Hints**
To create a semaphore: [`vkCreateSemaphore`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#vkCreateSemaphore).

## **The Code**
We need to create two kinds of semaphore. The first one is to check for image availability, and the second one for rendering availability. For both, we need to create as many semaphores as there are framebuffers.

To destroy each semaphore: [`vkDestroySemaphore`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap7.html#vkDestroySemaphore).

```C++
    /**************************************************************/
	/* Step 14.2: Semaphores (7.4.)                               */
	/**************************************************************/
    std::vector<VkSemaphore> image_available_sem_arr(framebuffer_arr.size());
	std::vector<VkSemaphore> render_finished_sem_arr(framebuffer_arr.size());
	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_info.pNext = nullptr;
	semaphore_info.flags = 0;

	for(size_t sem_i = 0; sem_i < image_available_sem_arr.size(); sem_i++){
		vk_result = vkCreateSemaphore(
			logical_device,
			&semaphore_info,
			nullptr,
			&image_available_sem_arr[sem_i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating semaphore!");
		}
		else {
			std::cout << "Semaphore created." << std::endl;
		}

		vk_result = vkCreateSemaphore(
			logical_device,
			&semaphore_info,
			nullptr,
			&render_finished_sem_arr[sem_i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating semaphore!");
		}
		else {
			std::cout << "Semaphore created." << std::endl;
		}
	}

```

File: [step_14_2_semaphores.cpp](../Code/step_14_2_semaphores.cpp)

| Previous | Next |
|---|---|
| [Step 14.1: Fences (7.3.)](fences.md) | [Step 15: Drawing (21.3. and 33.10.)](drawing.md) |