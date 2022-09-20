# **Step 1: Instances (4.2.)**
## **Hints**
To create an instance, the function needed is [`vkCreateInstance`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap4.html#vkCreateInstance). It will require to fill a [`VkInstanceCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap4.html#VkInstanceCreateInfo) structure to which we can point to a [`VkApplicationInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap4.html#VkApplicationInfo) structure (not mendatory).

## **The Code**
We can enable layers. I made the choice to print all the available layers and also choose the [`"VK_LAYER_KHRONOS_validation"`](https://vulkan.lunarg.com/doc/view/1.2.170.0/linux/khronos_validation_layer.html) so Vulkan display some of the error messages. This is not mendatory, one can chose not to have layers.

The function [`glfwGetRequiredInstanceExtensions`](https://www.glfw.org/docs/3.3/group__vulkan.html#ga99ad342d82f4a3421e2864978cb6d1d6) returns provides the extensions needed when using GLFW.

Do not forget to destroy the instance at the end with: [`vkDestroyInstance`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap4.html#vkDestroyInstance).

```C++
    /**************************************************************/
	/* Step 1: Instances (4.2.)                                   */
	/**************************************************************/
	VkInstance vk_instance{};
	VkApplicationInfo vk_application_info{};
	vk_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vk_application_info.pNext = nullptr;
	vk_application_info.pApplicationName = "hello world cube";
	vk_application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	vk_application_info.pEngineName = "no engine";
	vk_application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	vk_application_info.apiVersion = VK_API_VERSION_1_3;

	uint32_t glfw_extension_count = 0;
	const char** glfw_extensions;
	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	uint32_t instance_layer_prop_count = 0;
	std::vector<VkLayerProperties> layer_properties;
	vkEnumerateInstanceLayerProperties(&instance_layer_prop_count, nullptr);
	layer_properties.resize(instance_layer_prop_count);
	vkEnumerateInstanceLayerProperties(&instance_layer_prop_count, layer_properties.data());
	std::cout << "Available layers:" << std::endl;
	bool is_validation_layer = false;
	for (auto layer_prop : layer_properties) {
		std::string layer_name = std::string(layer_prop.layerName);
		std::cout << "\t" << layer_name << std::endl;
		is_validation_layer = is_validation_layer || (layer_name == "VK_LAYER_KHRONOS_validation");
	}
	if (!is_validation_layer) {
		throw std::runtime_error("Validation layer not found.");
	}

	std::vector<const char*> enabled_layers = {
		"VK_LAYER_KHRONOS_validation"
	};

	VkInstanceCreateInfo vk_instance_create_info{};
	vk_instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_instance_create_info.pNext = nullptr;
	vk_instance_create_info.flags = 0;
	vk_instance_create_info.pApplicationInfo = &vk_application_info;
	vk_instance_create_info.enabledLayerCount = enabled_layers.size();
	vk_instance_create_info.ppEnabledLayerNames = enabled_layers.data();
	vk_instance_create_info.enabledExtensionCount = glfw_extension_count;
	vk_instance_create_info.ppEnabledExtensionNames = glfw_extensions;

	vk_result = vkCreateInstance(&vk_instance_create_info, nullptr, &vk_instance);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Instance creation failed!");
	}
	std::cout << "Instance created successfully." << std::endl;
```

File: [step_1_instances.cpp](../Code/step_1_instances.cpp)

| Previous | Next |
|---|---|
| [Window creation with GLFW](context_initialisation_with_GLFW.md) | [Step 2: Surface](surface.md) |