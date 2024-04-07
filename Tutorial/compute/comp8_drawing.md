# **Comp 7: Dispatch (28.)**
## **Hints**

For drawing, the steps are the same as the one in [Step 15: Drawing (21.3. and 33.10.)](../drawing.md). We only need to adapt the semaphores and fences.

```C++
    /**************************************************************/
    /* Comp 8: Drawing (21.3. and 33.10.)                         */
    /**************************************************************/
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

    //---- Command Buffer Recording (6.4.)----//
    {
        VkCommandBufferBeginInfo command_buffer_begin_info{};
        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.pNext = nullptr;
        command_buffer_begin_info.flags = 0;
        command_buffer_begin_info.pInheritanceInfo = nullptr;

        vk_result = vkBeginCommandBuffer(
            command_buffer_arr[current_frame],
            &command_buffer_begin_info
        );
        if (vk_result != VK_SUCCESS) {
            throw std::runtime_error("Error: faild begining command buffer recording!");
        }

        //---- Begin the Render pass (8.4.)----//
        std::vector<VkClearValue> clear_value_arr(2);
        clear_value_arr[0].color.float32[0] = 0.2f;
        clear_value_arr[0].color.float32[1] = 0.2f;
        clear_value_arr[0].color.float32[2] = 0.2f;
        clear_value_arr[0].color.float32[3] = 1.0f;
        clear_value_arr[1].depthStencil.depth = 1.0f;
        clear_value_arr[1].depthStencil.stencil = 0;

        VkRenderPassBeginInfo render_pass_begin_info{};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.pNext = nullptr;
        render_pass_begin_info.renderPass = render_pass;
        render_pass_begin_info.framebuffer = framebuffer_arr[image_index];
        render_pass_begin_info.renderArea = scissor_arr[0];
        render_pass_begin_info.clearValueCount = clear_value_arr.size();
        render_pass_begin_info.pClearValues = clear_value_arr.data();

        vkCmdBeginRenderPass(
            command_buffer_arr[current_frame],
            &render_pass_begin_info,
            VK_SUBPASS_CONTENTS_INLINE
        );

        //---- Pipeline Binding (10.10.)----//
        vkCmdBindPipeline(
            command_buffer_arr[current_frame],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            graphic_pipeline
        );

        //---- Descriptor Sets Binding (14.2.7)----//
        vkCmdBindDescriptorSets(
            command_buffer_arr[current_frame],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline_layout,
            0,
            descriptor_sets_arr.size(),
            descriptor_sets_arr.data(),
            0,
            nullptr
        );

        //---- Vertex buffer binding ----//
        const VkDeviceSize buff_offset[1] = { 0 };
        vkCmdBindVertexBuffers(
            command_buffer_arr[current_frame],
            coord_buff_input_binding_desc.binding,
            1,
            &coord_buffer,
            buff_offset
        );

        vkCmdBindVertexBuffers(
            command_buffer_arr[current_frame],
            color_buff_input_binding_desc.binding,
            1,
            &color_buffer,
            buff_offset
        );

        //---- Set Viewport ---//
        vkCmdSetViewport(
            command_buffer_arr[current_frame],
            0,
            viewport_arr.size(),
            viewport_arr.data()
        );

        //---- Set Scissor (28.2.)----//
        vkCmdSetScissor(
            command_buffer_arr[current_frame],
            0,
            scissor_arr.size(),
            scissor_arr.data()
        );

        //---- Draw ----//
        vkCmdDraw(
            command_buffer_arr[current_frame],
            vert_coord.size(),
            1,
            0,
            0
        );

        // Ending Render Pass
        vkCmdEndRenderPass(command_buffer_arr[current_frame]);

        //---- End Command Buffer recording ----//
        vk_result = vkEndCommandBuffer(command_buffer_arr[current_frame]);
        if (vk_result != VK_SUCCESS) {
            throw std::runtime_error("Error: failed ending command buffer recording!");
        }

        //---- Submitting the command buffer----//
        std::vector<VkSemaphore> wait_semaphore_arr = { compute_end_sem_arr[current_frame] };
        std::vector<VkSemaphore> signal_semaphore_arr = { render_finished_sem_arr[current_frame] };

        std::vector<VkPipelineStageFlags> pipeline_stage_flags = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = nullptr;
        submit_info.waitSemaphoreCount = wait_semaphore_arr.size();
        submit_info.pWaitSemaphores = wait_semaphore_arr.data();
        submit_info.pWaitDstStageMask = pipeline_stage_flags.data();
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer_arr[current_frame];
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = signal_semaphore_arr.data();

        std::vector<VkSubmitInfo> submit_info_arr{ submit_info };

        vk_result = vkQueueSubmit(
            graphic_queue,
            submit_info_arr.size(),
            submit_info_arr.data(),
            in_flight_fence_arr[image_index]
        );
        if (vk_result != VK_SUCCESS) {
            throw std::runtime_error("Error: failed submitting queue!");
        }

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = nullptr;
        present_info.waitSemaphoreCount = signal_semaphore_arr.size();
        present_info.pWaitSemaphores = signal_semaphore_arr.data();
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &swapchain;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;

        vk_result = vkQueuePresentKHR(
            present_queue,
            &present_info
        );

        if (vk_result != VK_SUCCESS) {
            throw std::runtime_error("Error: failed presenting! :(");
        }
    }
```

File: [comp7_dispatch.cpp](../../Code/comp8_drawing.cpp)

| Previous |
|---|
| [Comp 7: Dispatch (28.)](comp7_dispatch.md) |
