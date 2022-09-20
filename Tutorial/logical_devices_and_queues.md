# **Step 3.2: Logical Devices and Queues (5.2. and 5.3.)**
## **Hints**
The goal is to create a logical device by calling the function [`vkCreateDevice`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#vkCreateDevice). From there, the structure [`VkDeviceCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#VkDeviceCreateInfo) will show us that it needs queues.

## **The Code**
First thing first, we need to retreive the existing queue family properties by calling [`vkGetPhysicalDeviceQueueFamilyProperties`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#VkDeviceCreateInfo). 

We will need two queues. For each of them, we need to iterate throug the properties found previously, and look for a family containing the the wanted flags in the `queueFlags` field of the [`VkQueueFamilyProperties`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#VkQueueFamilyProperties) structure.

Next, we need to define an array of extensions, here it contains only the value [`VK_KHR_SWAPCHAIN_EXTENSION_NAME`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap50.html#_new_enum_constants_37).

With the [`VkDeviceQueueCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#VkDeviceQueueCreateInfo) filled previously and the extensions' arrray, one can finaly fill the [`VkDeviceCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#VkDeviceCreateInfo) structure and then call [`vkCreateDevice`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#vkCreateDevice).

After creating the logical device, we need to get the actual queues with [`vkGetDeviceQueue`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#vkGetDeviceQueue).

```C++
    /**************************************************************/
	/* Step 3.2: Logical Devices and Queues (5.2 and 5.3)         */
	/**************************************************************/
	// Retreiving existing queue
	uint32_t queue_family_property_count;
	std::vector<VkQueueFamilyProperties> queue_family_propertie_arr;
	vkGetPhysicalDeviceQueueFamilyProperties(
		discrete_gpu,
		&queue_family_property_count,
		nullptr
	);
	queue_family_propertie_arr.resize(queue_family_property_count);
	vkGetPhysicalDeviceQueueFamilyProperties(
		discrete_gpu,
		&queue_family_property_count,
		queue_family_propertie_arr.data()
	);
	if (queue_family_propertie_arr.size() == 0) {
		throw std::runtime_error("Error: no queue family found!");
	}

	// For now, we want one queue family composed of one queue
	// GRAPHICS QUEUE
	VkQueue graphic_queue = nullptr;
	int32_t nb_graphics_queue_families_to_create = 1;
	std::vector<VkDeviceQueueCreateInfo> graphics_queues_arr(nb_graphics_queue_families_to_create);
	for (uint32_t i = 0; i < graphics_queues_arr.size(); i++) {
		if (!(queue_family_propertie_arr[i].queueFlags & VK_QUEUE_GRAPHICS_BIT
			&&
			queue_family_propertie_arr[i].queueFlags & VK_QUEUE_COMPUTE_BIT
			)) {
			continue;
		}

		std::vector<float> queue_priorities(1, 1.0f);

		VkDeviceQueueCreateInfo queue{};
		queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue.pNext = nullptr;
		queue.queueFamilyIndex = i;
		queue.queueCount = 1;
		queue.pQueuePriorities = queue_priorities.data();

		graphics_queues_arr[i] = queue;

		nb_graphics_queue_families_to_create--;
		if (nb_graphics_queue_families_to_create == 0) {
			break;
		}
	}
	if (nb_graphics_queue_families_to_create > 0) {
		throw std::runtime_error("Error: the right amount of queue was not found!");
	}

	// PRESENTATION QUEUE
	VkQueue present_queue = nullptr;
	int32_t nb_present_queue_families_to_create = 1;
	std::vector<VkDeviceQueueCreateInfo> present_queues_arr(nb_present_queue_families_to_create);
	for (uint32_t i = 0; i < present_queues_arr.size(); i++) {
		if (!(queue_family_propertie_arr[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
			continue;
		}
		VkBool32 supported;
		vkGetPhysicalDeviceSurfaceSupportKHR(
			discrete_gpu,
			i,
			surface,
			&supported
		);
		if (supported == VK_FALSE) {
			continue;
		}

		std::vector<float> queue_priorities(1, 1.0f);

		VkDeviceQueueCreateInfo queue{};
		queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue.pNext = nullptr;
		queue.queueFamilyIndex = i;
		queue.queueCount = 1;
		queue.pQueuePriorities = queue_priorities.data();

		present_queues_arr[i] = queue;

		nb_present_queue_families_to_create--;
		if (nb_present_queue_families_to_create == 0) {
			break;
		}
	}
	if (nb_present_queue_families_to_create > 0) {
		throw std::runtime_error("Error: the right amount of queue was not found!");
	}

	std::vector<VkDeviceQueueCreateInfo> queues_create_info{ graphics_queues_arr[0] };
	std::vector<uint32_t> queue_family_indexes = { graphics_queues_arr[0].queueFamilyIndex };

    // Checking if the found queue are in fact from the same family or not
	if (present_queues_arr[0].queueFamilyIndex != graphics_queues_arr[0].queueFamilyIndex) {
		queues_create_info.push_back(present_queues_arr[0]);
		queue_family_indexes.push_back(present_queues_arr[0].queueFamilyIndex);
	}

	// Creating the LOGICAL DEVICE
	const std::vector<const char*> device_extensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDevice logical_device;
	VkPhysicalDeviceFeatures device_features{};
	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pNext = nullptr;
	//device_create_info.flags;
	device_create_info.queueCreateInfoCount = queues_create_info.size();
	device_create_info.pQueueCreateInfos = queues_create_info.data();
	device_create_info.enabledExtensionCount = device_extensions.size();
	device_create_info.ppEnabledExtensionNames = device_extensions.data();
	device_create_info.pEnabledFeatures = &device_features;

	vk_result;
	vk_result = vkCreateDevice(
		discrete_gpu,
		&device_create_info,
		nullptr,
		&logical_device
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error creating logical device!");
	}
	else {
		std::cout << "Logical device created." << std::endl;
	}

	vkGetDeviceQueue(
		logical_device,
		graphics_queues_arr[0].queueFamilyIndex,
		0,
		&graphic_queue
	);
	vkGetDeviceQueue(
		logical_device,
		present_queues_arr[0]. ,
		0,
		&present_queue
	);
```

File: [step_3_2_logical_devices_and_queues.cpp](../Code/step_3_2_logical_devices_and_queues.cpp)

| Previous | Next |
|---|---|
| [Step 3.1: Physical Devices](physical_devices.md) | [Step 4: Command Buffers (6.)](command_buffers.md) |