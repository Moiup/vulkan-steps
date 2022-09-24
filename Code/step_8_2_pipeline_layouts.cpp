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

	/**************************************************************/
	/* Step 5: Swapchain and Image Views                          */
	/**************************************************************/
	/**************************************************************/
	/* Step 5.1: Swapchain (33.10.)                               */
	/**************************************************************/
	int32_t width_in_pixels, height_in_pixels;
	glfwGetFramebufferSize(window, &width_in_pixels, &height_in_pixels);

	VkSwapchainKHR swapchain{};
	VkSwapchainCreateInfoKHR swapchain_info{};
	swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.pNext = nullptr;
	swapchain_info.flags = 0;
	swapchain_info.surface = surface;
	swapchain_info.minImageCount = nb_frame;
	swapchain_info.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
	swapchain_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchain_info.imageExtent = { static_cast<uint32_t>(width_in_pixels), static_cast<uint32_t>(height_in_pixels) };
	swapchain_info.imageArrayLayers = 1;
	swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (queue_family_indexes.size() == 1) {
		swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	else {
		swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
	}
	swapchain_info.queueFamilyIndexCount = queue_family_indexes.size();
	swapchain_info.pQueueFamilyIndices = queue_family_indexes.data();
	swapchain_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchain_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	swapchain_info.clipped = VK_TRUE;
	swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	vk_result = vkCreateSwapchainKHR(
		logical_device,
		&swapchain_info,
		nullptr,
		&swapchain
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to create swapchain!");
	}
	std::cout << "Swapchain created." << std::endl;

	/**************************************************************/
	/* Step 5.2: Image Views (12.5.)                              */
	/**************************************************************/
	uint32_t swapchain_image_count;
	std::vector<VkImage> swapchain_image;
	vk_result = vkGetSwapchainImagesKHR(
		logical_device,
		swapchain,
		&swapchain_image_count,
		nullptr
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to access swapchain image count!");
	}
	std::cout << "image_count: " << swapchain_image_count << std::endl;

	swapchain_image.resize(swapchain_image_count);
	vk_result = vkGetSwapchainImagesKHR(
		logical_device,
		swapchain,
		&swapchain_image_count,
		swapchain_image.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to retrieve the swapchain images!");
	}

	std::vector<VkImageView> swapchain_image_view_arr(swapchain_image_count);
	for (size_t i = 0; i < swapchain_image_count; i++) {
		VkImageViewCreateInfo  swapchain_image_view_info{};
		swapchain_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		swapchain_image_view_info.pNext = nullptr;
		swapchain_image_view_info.flags = 0;
		swapchain_image_view_info.image = swapchain_image[i];
		swapchain_image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		swapchain_image_view_info.format = swapchain_info.imageFormat;
		swapchain_image_view_info.components.r = VK_COMPONENT_SWIZZLE_R;
		swapchain_image_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
		swapchain_image_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
		swapchain_image_view_info.components.a = VK_COMPONENT_SWIZZLE_A;
		swapchain_image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		swapchain_image_view_info.subresourceRange.baseMipLevel = 0;
		swapchain_image_view_info.subresourceRange.levelCount = 1;
		swapchain_image_view_info.subresourceRange.baseArrayLayer = 0;
		swapchain_image_view_info.subresourceRange.layerCount = 1;

		vk_result = vkCreateImageView(
			logical_device,
			&swapchain_image_view_info,
			nullptr,
			&swapchain_image_view_arr[i]
		);
	}

	/**************************************************************/
	/* Step 6: Depth Buffer (12.3., 12.5. and 12.7.)              */
	/**************************************************************/
	// Creating the image storing the depth buffer
	VkImage depth_buffer = nullptr;
	VkImageCreateInfo depth_buffer_info{};
	depth_buffer_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	depth_buffer_info.pNext = nullptr;
	depth_buffer_info.flags = 0;
	depth_buffer_info.imageType = VK_IMAGE_TYPE_2D;
	depth_buffer_info.format = VK_FORMAT_D16_UNORM;
	depth_buffer_info.extent = { static_cast<uint32_t>(width_in_pixels), static_cast<uint32_t>(height_in_pixels), 1 };
	depth_buffer_info.mipLevels = 1;
	depth_buffer_info.arrayLayers = 1;
	depth_buffer_info.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_buffer_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	depth_buffer_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depth_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	depth_buffer_info.queueFamilyIndexCount = queue_family_indexes.size();
	depth_buffer_info.pQueueFamilyIndices = queue_family_indexes.data();
	depth_buffer_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	vk_result = vkCreateImage(
		logical_device,
		&depth_buffer_info,
		nullptr,
		&depth_buffer
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the depth buffer!");
	}

	// Getting memory requirements
	VkMemoryRequirements depth_buffer_memory_req{};
	vkGetImageMemoryRequirements(
		logical_device,
		depth_buffer,
		&depth_buffer_memory_req
	);

	// Allocating the memory
	VkDeviceMemory depth_buffer_memory;
	VkMemoryAllocateInfo depth_buffer_image_memory_allocate_info{};
	depth_buffer_image_memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	depth_buffer_image_memory_allocate_info.pNext = nullptr;
	depth_buffer_image_memory_allocate_info.allocationSize = depth_buffer_memory_req.size;
	depth_buffer_image_memory_allocate_info.memoryTypeIndex = -1;

	VkPhysicalDeviceMemoryProperties memory_properties{};
	vkGetPhysicalDeviceMemoryProperties(discrete_gpu, &memory_properties);
	std::cout << "Memory type count: " << memory_properties.memoryTypeCount << std::endl;
	uint32_t memory_type_i = 0;
	VkMemoryPropertyFlags memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((depth_buffer_memory_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			depth_buffer_image_memory_allocate_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: depth buffer memory type not found!");
	}

	vk_result = vkAllocateMemory(
		logical_device,
		&depth_buffer_image_memory_allocate_info,
		nullptr,
		&depth_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("error: failed allocating depth buffer memory!");
	}

	// Binding the memory
	vkBindImageMemory(
		logical_device,
		depth_buffer,
		depth_buffer_memory,
		0
	);

	// Creating the image view
	VkImageView depth_image_view{};
	VkImageViewCreateInfo depth_image_view_info{};
	depth_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depth_image_view_info.pNext = nullptr;
	depth_image_view_info.flags = 0;
	depth_image_view_info.image = depth_buffer;
	depth_image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depth_image_view_info.format = VK_FORMAT_D16_UNORM;
	depth_image_view_info.components.r = VK_COMPONENT_SWIZZLE_R;
	depth_image_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
	depth_image_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
	depth_image_view_info.components.a = VK_COMPONENT_SWIZZLE_A;
	depth_image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	depth_image_view_info.subresourceRange.baseMipLevel = 0;
	depth_image_view_info.subresourceRange.levelCount = 1;
	depth_image_view_info.subresourceRange.baseArrayLayer = 0;
	depth_image_view_info.subresourceRange.layerCount = 1;

	vkCreateImageView(
		logical_device,
		&depth_image_view_info,
		nullptr,
		&depth_image_view
	);
	std::cout << "Depth buffer created." << std::endl;

	/**************************************************************/
	/* Step 7: Depth Buffer (12.1., and 12.7.)                    */
	/**************************************************************/
	// Setting the MVP Matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(
		glm::vec3(-5, 3, -10),
		glm::vec3(0, 0, 0),
		glm::vec3(0, -1, 0)
	);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 clip = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f, 1.0f
	);

	glm::mat4 mvp = clip * projection * view * model;

	// Creating the uniform buffer
	VkBuffer uniform_buffer = nullptr;
	VkBufferCreateInfo uniform_buffer_info{};
	uniform_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	uniform_buffer_info.pNext = nullptr;
	uniform_buffer_info.flags = 0;
	uniform_buffer_info.size = sizeof(mvp);
	uniform_buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	uniform_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	uniform_buffer_info.queueFamilyIndexCount = 0;
	uniform_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		logical_device,
		&uniform_buffer_info,
		nullptr,
		&uniform_buffer
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating uniform buffer!");
	}

	// Requiring the memory information
	VkMemoryRequirements uniform_buffer_memory_req{};
	vkGetBufferMemoryRequirements(
		logical_device,
		uniform_buffer,
		&uniform_buffer_memory_req
	);

	// Allocating the memory
	VkDeviceMemory uniform_buffer_memory = nullptr;
	VkMemoryAllocateInfo uniform_buffer_memory_info{};
	uniform_buffer_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	uniform_buffer_memory_info.pNext = nullptr;
	uniform_buffer_memory_info.allocationSize = uniform_buffer_memory_req.size;
	uniform_buffer_memory_info.memoryTypeIndex = 0;

	// No need to call vkGetPhysicalDeviceMemoryProperties again, we did it when allocating the memory for the depth buffer
	memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((uniform_buffer_memory_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			uniform_buffer_memory_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: uniform buffer memory type not found!");
	}

	vk_result = vkAllocateMemory(
		logical_device,
		&uniform_buffer_memory_info,
		nullptr,
		&uniform_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating uniform buffer memory!");
	}

	// Mapping the memory
	float* mvp_copy = new float[4 * 4];
	vk_result = vkMapMemory(
		logical_device,
		uniform_buffer_memory,
		0,
		uniform_buffer_memory_req.size,
		0,
		(void**)&mvp_copy
	);
	memcpy(mvp_copy, &mvp, 4 * 4 * sizeof(float));
	vkUnmapMemory(logical_device, uniform_buffer_memory);

	// Binding the memory
	vk_result = vkBindBufferMemory(
		logical_device,
		uniform_buffer,
		uniform_buffer_memory,
		0
	);
	std::cout << "Uniform buffer created." << std::endl;

	/**************************************************************/
	/* Step 8: Descriptor Sets (14.2.)                            */
	/**************************************************************/
	/**************************************************************/
	/* Step 8.1: Descriptor Set Layout (14.2.1.)                  */
	/**************************************************************/
	VkDescriptorSetLayoutBinding descriptor_set_binding;
	descriptor_set_binding.binding = 0;
	descriptor_set_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptor_set_binding.descriptorCount = 1;
	descriptor_set_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	descriptor_set_binding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> descriptor_set_binding_arr = { descriptor_set_binding };

	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
	descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_info.pNext = nullptr;
	descriptor_set_layout_info.flags = 0;
	descriptor_set_layout_info.bindingCount = descriptor_set_binding_arr.size();
	descriptor_set_layout_info.pBindings = descriptor_set_binding_arr.data();

	std::vector<VkDescriptorSetLayout> descriptor_set_layout_arr(descriptor_set_binding_arr.size());
	vk_result = vkCreateDescriptorSetLayout(
		logical_device,
		&descriptor_set_layout_info,
		nullptr,
		descriptor_set_layout_arr.data()
	);

	/**************************************************************/
	/* Step 8.2: Pipeline Layouts (14.2.2.)                       */
	/**************************************************************/
	VkPipelineLayout pipeline_layout = nullptr;
	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.pNext = nullptr;
	pipeline_layout_info.flags = 0;
	pipeline_layout_info.setLayoutCount = descriptor_set_layout_arr.size();
	pipeline_layout_info.pSetLayouts = descriptor_set_layout_arr.data();
	pipeline_layout_info.pushConstantRangeCount = 0;
	pipeline_layout_info.pPushConstantRanges = nullptr;
	vk_result = vkCreatePipelineLayout(
		logical_device,
		&pipeline_layout_info,
		nullptr,
		&pipeline_layout
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the pipeline layout!");
	}
	
	/********************/
	/* Game Loop        */
	/********************/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	/**************/
	/* Destroying */
	/**************/
	// Descriptor set Layout
	for (auto dl : descriptor_set_layout_arr) {
		vkDestroyDescriptorSetLayout(logical_device, dl, nullptr);
	}
	// Pipeline layout
	vkDestroyPipelineLayout(logical_device, pipeline_layout, nullptr);
	// Uniform buffer
	vkFreeMemory(logical_device, uniform_buffer_memory, nullptr);
	vkDestroyBuffer(logical_device, uniform_buffer, nullptr);
	// Depth buffer
	vkDestroyImageView(logical_device, depth_image_view, nullptr);
	vkFreeMemory(logical_device, depth_buffer_memory, nullptr);
	vkDestroyImage(logical_device, depth_buffer, nullptr);
	// Swapchain
	for (auto siv : swapchain_image_view_arr) {
		vkDestroyImageView(logical_device, siv, nullptr);
	}
	vkDestroySwapchainKHR(logical_device, swapchain, nullptr);
	// Command Buffersr
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
