# **Comp 7: Dispatch (28.)**
## **Hints**
To make the computation using the compute shader, we must call [`vkCmdDispatch`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkCmdDispatch).

## **The Code**

As we want to execute the compute shader before rendering but in the same iteration, we first need to achquire an image with [`vkAcquireNextImageKHR`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkAcquireNextImageKHR).

Then, we start the actual Dispatching process by waiting first for the fences and reseting them, afterwards we start the command buffer and do the different bindings (pipeline and descriptor set). Finally we can dispatch and end the command buffer.
    - [`vkWaitForFences`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkWaitForFences)
    - [`vkResetFences`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkResetFences)
    - [`vkBeginCommandBuffer`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkBeginCommandBuffer)
    - [`vkCmdBindPipeline`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkCmdBindPipeline)
    - [`vkCmdBindDescriptorSets`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkCmdBindDescriptorSets)
    - [`vkCmdDispatch`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkCmdDispatch)
    - [`vkEndCommandBuffer`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkEndCommandBuffer)

The last step consists in filling a [`VkSubmitInfo`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#VkSubmitInfo) structure and submitting queue with [`vkQueueSubmit`](https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/vkspec.html#vkQueueSubmit).

```C++
        // Get available image (to draw into)
		uint32_t image_index{ 0 };

		vk_result = vkAcquireNextImageKHR(
			logical_device,
			swapchain,
			UINT64_MAX,
			acquired_image_sem_arr[current_frame],
			VK_NULL_HANDLE,
			&image_index
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed acquiring next image!");
		}

		/**************************************************************/
		/* Comp 7: Dispatch (28.)                                     */
		/**************************************************************/
		VkCommandBufferBeginInfo command_buffer_begin_info{};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.pNext = nullptr;
		command_buffer_begin_info.flags = 0;
		command_buffer_begin_info.pInheritanceInfo = nullptr;

		vkWaitForFences(
			logical_device,
			1,
			&in_flight_fence_arr[current_frame],
			VK_TRUE,
			UINT64_MAX
		);
		vkResetFences(
			logical_device,
			1,
			&in_flight_fence_arr[current_frame]
		);

		vk_result = vkBeginCommandBuffer(
			command_buffer_arr[current_frame],
			&command_buffer_begin_info
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: faild begining command buffer recording!");
		}

		vkCmdBindPipeline(
			command_buffer_arr[current_frame],
			VK_PIPELINE_BIND_POINT_COMPUTE,
			compute_pipelines_arr[0]
		);

		vkCmdBindDescriptorSets(
			command_buffer_arr[current_frame],
			VK_PIPELINE_BIND_POINT_COMPUTE,
			compute_pipeline_layout,
			0,
			compute_descriptor_sets_arr.size(),
			compute_descriptor_sets_arr.data(),
			0,
			nullptr
		);

		vkCmdDispatch(
			command_buffer_arr[current_frame],
			100,
			1,
			1
		);

		vk_result = vkEndCommandBuffer(command_buffer_arr[current_frame]);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed ending command buffer recording!");
		}

		std::vector<VkPipelineStageFlags> compute_pipeline_stage_flags{
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
		};
		VkSubmitInfo compute_submit_info{};
		compute_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		compute_submit_info.pNext = nullptr;
		compute_submit_info.waitSemaphoreCount = 1;
		compute_submit_info.pWaitSemaphores = &acquired_image_sem_arr[current_frame];
		compute_submit_info.pWaitDstStageMask = compute_pipeline_stage_flags.data();
		compute_submit_info.commandBufferCount = 1;
		compute_submit_info.pCommandBuffers = &command_buffer_arr[current_frame];
		compute_submit_info.signalSemaphoreCount = 1;
		compute_submit_info.pSignalSemaphores = &compute_end_sem_arr[current_frame];

		std::vector<VkSubmitInfo> compute_submit_info_arr{ compute_submit_info };

		vk_result = vkQueueSubmit(
			compute_queue,
			compute_submit_info_arr.size(),
			compute_submit_info_arr.data(),
			in_flight_fence_arr[current_frame]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed submitting queue when dispatching!");
		}
```

File: [comp7_dispatch.cpp](../../Code/comp7_dispatch.cpp)

| Previous | Next |
|---|---|
| [Comp 6.2: Semaphores (7.4.)](comp7_semaphores.md) | [Comp 8: Drawing (21.3. and 33.10.)](comp8_drawing.md) |
