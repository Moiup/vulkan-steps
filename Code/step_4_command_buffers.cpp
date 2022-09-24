#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <vulkan/vulkan.h>

int main()
{
	/**************************************************************/
	/* Step 0: Window creation with GLFW                          */
	/**************************************************************/
	VkResult vk_result = VK_SUCCESS;

	const uint64_t width{ 1280 };
	const uint64_t height{ 720 };

	GLFWwindow* window;

	std::cout << "Initializing GLFW..." << std::endl;
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialise GLFW");
	}
	std::cout << "GLFW initialized with success." << std::endl;

	std::cout << "GLFW window creation." << std::endl;
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, "Le Cube", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		throw std::runtime_error("Window creation failed.");
	}
	std::cout << "GLFW window created successfully." << std::endl;

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

	/**************************************************************/
	/* Step 2: Surface                                            */
	/**************************************************************/
	VkSurfaceKHR surface;
	vk_result = glfwCreateWindowSurface(
		vk_instance,
		window,
		nullptr,
		&surface
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to create window surface.");
	}

	/**************************************************************/
	/* Step 3: Devices and Queues (5.)                            */
	/**************************************************************/
	/**************************************************************/
	/* Step 3.1: Physical Devices (5.1)                           */
	/**************************************************************/
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
	for (uint32_t i = 0; i < queue_family_property_count; i++) {
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
	for (uint32_t i = 0; i < queue_family_property_count; i++) {
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
		present_queues_arr[0].queueFamilyIndex,
		0,
		&present_queue
	);

	/**************************************************************/
	/* Step 4: Command Buffers (6.)                               */
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

	/********************/
	/* Game Loop        */
	/********************/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	
	/**************/
	/* Destroying */
	/**************/
	// Command Buffers
	vkFreeCommandBuffers(
		logical_device,
		command_pool,
		command_buffer_arr.size(),
		command_buffer_arr.data()
	);
	// Command Pool
	vkDestroyCommandPool(logical_device, command_pool, nullptr);
	// Logical device
	vkDestroyDevice(logical_device, nullptr);
	// Surface
	vkDestroySurfaceKHR(vk_instance, surface, nullptr);
	// Instance
	vkDestroyInstance(vk_instance, nullptr);

	glfwTerminate();
	std::cout << "Programme terminated successfully" << std::endl;
}
