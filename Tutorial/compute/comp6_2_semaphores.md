# **Comp 6.2: Semaphores (7.4.)**

We will need to create multiple semaphores.
- A first semaphore to signal that we acquired an image.
- A semaphore to signal the start of computing.
- A semaphore to signal the end of computing.
- A semaphore to signal the availability of an image for rendering.
- A semaphore to signal that the rendering is finished.

```C++
    /**************************************************************/
	/* Comp 6.2: Semaphores (7.4.)                                */
	/**************************************************************/
	std::vector<VkSemaphore> acquired_image_sem_arr(framebuffer_arr.size());

	std::vector<VkSemaphore> compute_start_sem_arr(framebuffer_arr.size());
	std::vector<VkSemaphore> compute_end_sem_arr(framebuffer_arr.size());

	std::vector<VkSemaphore> image_available_sem_arr(framebuffer_arr.size());
	std::vector<VkSemaphore> render_finished_sem_arr(framebuffer_arr.size());
	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_info.pNext = nullptr;
	semaphore_info.flags = 0;

	for (size_t sem_i = 0; sem_i < image_available_sem_arr.size(); sem_i++) {
		// Acquired image
		vk_result = vkCreateSemaphore(
			logical_device,
			&semaphore_info,
			nullptr,
			&acquired_image_sem_arr[sem_i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating semaphore!");
		}
		else {
			std::cout << "Semaphore created." << std::endl;
		}

		// Compute start
		vk_result = vkCreateSemaphore(
			logical_device,
			&semaphore_info,
			nullptr,
			&compute_start_sem_arr[sem_i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating semaphore!");
		}
		else {
			std::cout << "Semaphore created." << std::endl;
		}

		// Compute end
		vk_result = vkCreateSemaphore(
			logical_device,
			&semaphore_info,
			nullptr,
			&compute_end_sem_arr[sem_i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating semaphore!");
		}
		else {
			std::cout << "Semaphore created." << std::endl;
		}

		// Image available
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

		// Render finished
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

| Previous | Next |
|---|---|
| [Comp 6.1: Fences (7.3.)](comp6_1_fences.md) | [Comp 7: Dispatch](comp7_dispatch.md) |
