# **Step 3.1: Physical Devices (5.1.)**
## **Hints**
To find a physical device, we need to enumerate them with [`vkEnumeratePhysicalDevices`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#vkEnumeratePhysicalDevices).

## **The Code**
We need first to enumerate the physical devices with [`vkEnumeratePhysicalDevices`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#vkEnumeratePhysicalDevices). Then, from this list, find the one that match the criteria we want. For each physical device, we need to access their properties with [`vkGetPhysicalDeviceProperties`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#vkGetPhysicalDeviceProperties). In this case, we are just checking for the `deviceType` field to be a [`VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap5.html#VkPhysicalDeviceType). If you have multiple GPU inside your computer, you might need other criteria to select the right one.

```C++
    /********************************************/
	/* Step 2.1: Physical Devices (5.1)         */
	/********************************************/
	std::vector<VkPhysicalDevice> physical_devices;
	uint32_t physical_device_count = 0;

	// Get the count
	vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, nullptr);
	vk_result = vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, physical_devices.data());
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Unable to count physical devices.");
	}
	// Get the actual array of devices
	physical_devices.resize(physical_device_count);
	vk_result = vkEnumeratePhysicalDevices(vk_instance, &physical_device_count, physical_devices.data());
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Unable to enumerate physical devices.");
	}

	std::cout << physical_devices.size() << std::endl;
	VkPhysicalDevice discrete_gpu{};
	VkPhysicalDeviceProperties discrete_gpu_properties;
	for (auto physical_device_tmp : physical_devices) {
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physical_device_tmp, &properties);
		std::cout << "Device: " << std::endl;
		std::cout << "\t";
		std::cout << properties.deviceName << std::endl;
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			discrete_gpu = physical_device_tmp;
			discrete_gpu_properties = properties;
		}
	}
	std::cout << "Selected physical device: " << std::endl;
	std::cout << "\t" << discrete_gpu_properties.deviceName << std::endl;
```

File: [step_3_1_physical_devices.cpp](../Code/step_3_1_physical_devices.cpp)

| Previous | Next |
|---|---|
| [Step 3: Devices and Queues (5.)](devices_and_queues.md) | [Step 3.2: Logical Devices and Queues (5.2. and 5.3.)](logical_devices_and_queues.md) |