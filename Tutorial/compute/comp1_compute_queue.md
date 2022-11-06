# **Comp 1: Compute Queue (5.3)**
## **Hints**
When creating the logical device, we created a graphic queue. Here, we want this same logical device to also have a compute queue. The objectif is to correctly fill the structure [`VkDeviceQueueCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#devsandqueues-queue-creation)

## **The Code**
First thing first, we need to retreive the existing queue family properties, but to change a little bit, we are going to use [`vkGetPhysicalDeviceQueueFamilyProperties2`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#vkGetPhysicalDeviceQueueFamilyProperties2).

We need to iterate through the properties found previously and look for a family containing the wanted flags in the `queueFlags` field of the [`VkQueueFamilyProperties`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#VkQueueFamilyProperties) structure.



This must be written before the logical device creation.
```C++
	/**************************************************************/
	/* Comp 1: Compute Queue (5.3.)                               */
	/**************************************************************/
	VkQueue compute_queue{};
	uint32_t compute_queue_family_properties_count = 0;
	std::vector<VkQueueFamilyProperties2> compute_queue_family_properties_arr;
	vkGetPhysicalDeviceQueueFamilyProperties2(
		discrete_gpu,
		&compute_queue_family_properties_count,
		nullptr
	);
	compute_queue_family_properties_arr.resize(compute_queue_family_properties_count);
	for (auto& cqfp : compute_queue_family_properties_arr) {
		cqfp.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
	}
	vkGetPhysicalDeviceQueueFamilyProperties2(
		discrete_gpu,
		&compute_queue_family_properties_count,
		compute_queue_family_properties_arr.data()
	);

	const float compute_queue_priority[1] = { 1.0f };
	VkDeviceQueueCreateInfo compute_queue_info{};
	compute_queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	compute_queue_info.pNext = nullptr;
	compute_queue_info.flags = 0;
	compute_queue_info.queueCount = 1;
	compute_queue_info.pQueuePriorities = compute_queue_priority;

	for (uint32_t i = 0; i < compute_queue_family_properties_arr.size(); i++) {
		auto prop = compute_queue_family_properties_arr[i];
		if (prop.queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			break;
		}

		compute_queue_info.queueFamilyIndex = i;
	}

    // Checking if the compute queue is from the same family as the other queues already found or not.
	uint32_t qf_i_found = -1;
	for (uint32_t q : queue_family_indexes) {
		if (q == compute_queue_info.queueFamilyIndex) {
			qf_i_found = q;
			break;
		}
	}
	if (qf_i_found == -1) {
		queues_create_info.push_back(compute_queue_info);
		queue_family_indexes.push_back(compute_queue_info.queueFamilyIndex);
	}
```

This must be written after the logical device creation.
```C++
    vkGetDeviceQueue(
		logical_device,
		compute_queue_info.queueFamilyIndex,
		0,
		&compute_queue
	);
```

File: [comp1_compute_queue.cpp](../../Code/comp1_compute_queue.cpp)

| Previous | Next |
|---|---|
| [Introduction](../../README.md) | [Comp 2: Compute Shader](comp2_compute_shader.md) |