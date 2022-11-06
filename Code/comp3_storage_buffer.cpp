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

static std::vector<char> readFile(const std::string& file_name) {
	size_t file_size{};
	std::vector<char> buffer;

	std::ifstream file(file_name, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Error: faile to open file: " + file_name);
	}

	file_size = (size_t)file.tellg();
	buffer.resize(file_size);

	file.seekg(0);
	file.read(buffer.data(), file_size);

	file.close();

	return buffer;
}

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

	/**************************************************************/
	/* Comp 1: Compute Queue (5.3)                                */
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
	vkGetDeviceQueue(
		logical_device,
		compute_queue_info.queueFamilyIndex,
		0,
		&compute_queue
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
	float* mvp_copy;
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

	/**************************************************************/
	/* Step 8.3: Allocation of Descriptor Sets (14.2.3.)          */
	/**************************************************************/
	// descriptor pool
	VkDescriptorPoolSize descriptor_pool_size{};
	descriptor_pool_size.type = descriptor_set_binding_arr[0].descriptorType;
	descriptor_pool_size.descriptorCount = 1;

	std::vector<VkDescriptorPoolSize> descriptor_pool_size_arr{ descriptor_pool_size };

	VkDescriptorPool descriptor_pool = nullptr;
	VkDescriptorPoolCreateInfo descriptor_pool_info{};
	descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_info.pNext = nullptr;
	descriptor_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptor_pool_info.maxSets = 1;
	descriptor_pool_info.poolSizeCount = descriptor_pool_size_arr.size();
	descriptor_pool_info.pPoolSizes = descriptor_pool_size_arr.data();

	vk_result = vkCreateDescriptorPool(
		logical_device,
		&descriptor_pool_info,
		nullptr,
		&descriptor_pool
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the descriptor pool.");
	}

	// Allocating descriptor sets
	std::vector<VkDescriptorSet> descriptor_sets_arr(descriptor_pool_size_arr.size());
	std::vector<VkDescriptorSetAllocateInfo> descriptor_sets_info_arr(descriptor_pool_size_arr.size());
	descriptor_sets_info_arr[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_sets_info_arr[0].pNext = nullptr;
	descriptor_sets_info_arr[0].descriptorPool = descriptor_pool;
	descriptor_sets_info_arr[0].descriptorSetCount = descriptor_pool_size_arr.size();
	descriptor_sets_info_arr[0].pSetLayouts = descriptor_set_layout_arr.data();

	vk_result = vkAllocateDescriptorSets(
		logical_device,
		descriptor_sets_info_arr.data(),
		descriptor_sets_arr.data()
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating descriptor sets.");
	}


	/**************************************************************/
	/* Step 8.4: Descriptor Set Updates (14.2.4.)                 */
	/**************************************************************/
	std::vector<VkDescriptorBufferInfo> descriptor_buffer_info(1);
	descriptor_buffer_info[0].buffer = uniform_buffer;
	descriptor_buffer_info[0].offset = 0;
	descriptor_buffer_info[0].range = VK_WHOLE_SIZE;

	std::vector<VkWriteDescriptorSet> descriptor_writes_arr(descriptor_sets_arr.size());
	descriptor_writes_arr[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptor_writes_arr[0].pNext = nullptr;
	descriptor_writes_arr[0].dstSet = descriptor_sets_arr[0];
	descriptor_writes_arr[0].dstBinding = descriptor_set_binding_arr[0].binding;
	descriptor_writes_arr[0].dstArrayElement = 0;
	descriptor_writes_arr[0].descriptorCount = 1;
	descriptor_writes_arr[0].descriptorType = descriptor_set_binding_arr[0].descriptorType;
	descriptor_writes_arr[0].pImageInfo = nullptr;
	descriptor_writes_arr[0].pBufferInfo = descriptor_buffer_info.data();
	descriptor_writes_arr[0].pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(
		logical_device,
		descriptor_writes_arr.size(),
		descriptor_writes_arr.data(),
		0,
		nullptr
	);

	/**************************************************************/
	/* Step 9: Descriptor Set Updates (8.1.)                      */
	/**************************************************************/
	// attachment description
	std::vector<VkAttachmentDescription> attachment_description_arr(2);
	// Color attachment
	attachment_description_arr[0].flags = 0;
	attachment_description_arr[0].format = swapchain_info.imageFormat;
	attachment_description_arr[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachment_description_arr[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_arr[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_description_arr[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_arr[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_arr[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_arr[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// Depth attachment
	attachment_description_arr[1].flags = 0;
	attachment_description_arr[1].format = depth_buffer_info.format;
	attachment_description_arr[1].samples = depth_buffer_info.samples;
	attachment_description_arr[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description_arr[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_arr[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description_arr[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description_arr[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment_description_arr[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// Subpass
	// Color attachment
	std::vector<VkAttachmentReference> color_attachments_arr(1);
	color_attachments_arr[0].attachment = 0;
	color_attachments_arr[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	// Depth attachment
	std::vector<VkAttachmentReference> depth_attachments_arr(1);
	depth_attachments_arr[0].attachment = 1;
	depth_attachments_arr[0].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::vector<VkSubpassDescription> subpass_description_arr(1);
	subpass_description_arr[0].flags = 0;
	subpass_description_arr[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_description_arr[0].inputAttachmentCount = 0;
	subpass_description_arr[0].pInputAttachments = nullptr;
	subpass_description_arr[0].colorAttachmentCount = color_attachments_arr.size();
	subpass_description_arr[0].pColorAttachments = color_attachments_arr.data();
	subpass_description_arr[0].pResolveAttachments = nullptr;
	subpass_description_arr[0].pDepthStencilAttachment = depth_attachments_arr.data();
	subpass_description_arr[0].preserveAttachmentCount = 0;
	subpass_description_arr[0].pPreserveAttachments = nullptr;

	// Subpass dependency
	std::vector<VkSubpassDependency> subpass_dependency_arr(1);
	subpass_dependency_arr[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dependency_arr[0].dstSubpass = 0;
	subpass_dependency_arr[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency_arr[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency_arr[0].srcAccessMask = 0;
	subpass_dependency_arr[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpass_dependency_arr[0].dependencyFlags = 0;

	// Render Pass
	VkRenderPass render_pass = nullptr;
	VkRenderPassCreateInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.pNext = nullptr;
	render_pass_info.flags = 0;
	render_pass_info.attachmentCount = attachment_description_arr.size();
	render_pass_info.pAttachments = attachment_description_arr.data();
	render_pass_info.subpassCount = subpass_description_arr.size();
	render_pass_info.pSubpasses = subpass_description_arr.data();
	render_pass_info.dependencyCount = subpass_dependency_arr.size();
	render_pass_info.pDependencies = subpass_dependency_arr.data();

	vk_result = vkCreateRenderPass(
		logical_device,
		&render_pass_info,
		nullptr,
		&render_pass
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating render pass!");
	}
	std::cout << "Render pass created." << std::endl;

	/**************************************************************/
	/* Step 10: Shaders (9.)                                      */
	/**************************************************************/
	/**************************************************************/
	/* Step 10.2: Shader Modules (9.1)                            */
	/**************************************************************/
	// Vertex shader
	std::vector<char> vertex_shader_text = readFile("shaders/vert.spv");
	VkShaderModule vertex_shader_module = nullptr;
	VkShaderModuleCreateInfo vertex_shader_module_info{};
	vertex_shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertex_shader_module_info.pNext = nullptr;
	vertex_shader_module_info.flags = 0;
	vertex_shader_module_info.codeSize = vertex_shader_text.size();
	vertex_shader_module_info.pCode = reinterpret_cast<const uint32_t*>(vertex_shader_text.data());

	vk_result = vkCreateShaderModule(
		logical_device,
		&vertex_shader_module_info,
		nullptr,
		&vertex_shader_module
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating vertex shader module!");
	}
	else {
		std::cout << "Vertex shader module created." << std::endl;
	}

	VkPipelineShaderStageCreateInfo vertex_pipeline{};
	vertex_pipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertex_pipeline.pNext = nullptr;
	vertex_pipeline.flags = 0;
	vertex_pipeline.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertex_pipeline.module = vertex_shader_module;
	vertex_pipeline.pName = "main";
	vertex_pipeline.pSpecializationInfo = nullptr;

	// Fragment shader
	std::vector<char> fragment_shader_text = readFile("shaders/frag.spv");
	VkShaderModule fragment_shader_module = nullptr;
	VkShaderModuleCreateInfo fragment_shader_module_info{};
	fragment_shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragment_shader_module_info.pNext = nullptr;
	fragment_shader_module_info.flags = 0;
	fragment_shader_module_info.codeSize = fragment_shader_text.size();
	fragment_shader_module_info.pCode = reinterpret_cast<const uint32_t*>(fragment_shader_text.data());

	vk_result = vkCreateShaderModule(
		logical_device,
		&fragment_shader_module_info,
		nullptr,
		&fragment_shader_module
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating fragment shader module!");
	}
	else {
		std::cout << "Fragment shader created." << std::endl;
	}

	VkPipelineShaderStageCreateInfo fragment_pipeline{};
	fragment_pipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragment_pipeline.pNext = nullptr;
	fragment_pipeline.flags = 0;
	fragment_pipeline.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragment_pipeline.module = fragment_shader_module;
	fragment_pipeline.pName = "main";
	fragment_pipeline.pSpecializationInfo = nullptr;

	// Putting both pipeline into an array
	std::vector<VkPipelineShaderStageCreateInfo> shader_stage_arr = {
		vertex_pipeline,
		fragment_pipeline
	};
	std::cout << "Shaders created." << std::endl;

	/**************************************************************/
	/* Step 11: Framebuffers (8.3.)                               */
	/**************************************************************/
	std::vector<VkFramebuffer> framebuffer_arr;

	for (size_t i = 0; i < swapchain_image_view_arr.size(); i++) {
		std::vector<VkImageView> attachments;

		attachments.push_back(swapchain_image_view_arr[i]);
		attachments.push_back(depth_image_view);

		VkFramebuffer framebuffer = nullptr;
		VkFramebufferCreateInfo framebuffer_info{};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.pNext = nullptr;
		framebuffer_info.flags = 0;
		framebuffer_info.renderPass = render_pass;
		framebuffer_info.attachmentCount = attachments.size();
		framebuffer_info.pAttachments = attachments.data();
		framebuffer_info.width = swapchain_info.imageExtent.width;
		framebuffer_info.height = swapchain_info.imageExtent.height;
		framebuffer_info.layers = 1;

		vk_result = vkCreateFramebuffer(
			logical_device,
			&framebuffer_info,
			nullptr,
			&framebuffer
		);

		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create framebuffer!");
		}

		framebuffer_arr.push_back(framebuffer);
	}
	std::cout << "Framebuffer created." << std::endl;

	/**************************************************************/
	/* Step 12: Vertex Buffer (12.1. and 12.7.)                   */
	/**************************************************************/
		/**************************************************************/
	/* Step 12.1: Data                                            */
	/**************************************************************/
	// Creating the vertex
	// Coordinate
	std::vector<glm::vec3> vert_coord = {
		// Front
		glm::vec3(1,  1, -1),
		glm::vec3(-1, -1, -1),
		glm::vec3(-1,  1, -1),
		glm::vec3(-1, -1, -1),
		glm::vec3(1,  1, -1),
		glm::vec3(1, -1, -1),
		// Right
		glm::vec3(1, -1, -1),
		glm::vec3(1,  1,  1),
		glm::vec3(1,  1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(1, -1,  1),
		glm::vec3(1,  1,  1),
		// Back
		glm::vec3(-1, -1,  1),
		glm::vec3(1, -1,  1),
		glm::vec3(1,  1,  1),
		glm::vec3(-1, -1,  1),
		glm::vec3(1,  1,  1),
		glm::vec3(-1,  1,  1),
		// Left
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, -1,  1),
		glm::vec3(-1,  1,  1),
		glm::vec3(-1, -1, -1),
		glm::vec3(-1,  1,  1),
		glm::vec3(-1,  1, -1),
		// Bottom
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, -1,  1),
		glm::vec3(1, -1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(-1, -1,  1),
		glm::vec3(1, -1,  1),
		// Top
		glm::vec3(-1,  1, -1),
		glm::vec3(-1,  1,  1),
		glm::vec3(1,  1,  1),
		glm::vec3(-1,  1, -1),
		glm::vec3(1,  1,  1),
		glm::vec3(1,  1, -1)
	};
	// Color
	std::vector<glm::vec4> vert_color = {
		// Front
		glm::vec4(1, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		// Right
		glm::vec4(0, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		// Back
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0, 1, 0, 1),
		// Left
		glm::vec4(0, 1, 1, 1),
		glm::vec4(0, 1, 1, 1),
		glm::vec4(0, 1, 1, 1),
		glm::vec4(0, 1, 1, 1),
		glm::vec4(0, 1, 1, 1),
		glm::vec4(0, 1, 1, 1),
		// Bottom
		glm::vec4(1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		// Top
		glm::vec4(1, 1, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(1, 1, 0, 1)
	};

	/**************************************************************/
	/* Step 12.2: Coordinate and Color Buffers (12.1. and 12.7.)  */
	/**************************************************************/
	// Creating the COORD BUFFER
	VkBuffer coord_buffer = nullptr;
	VkBufferCreateInfo coord_buffer_info{};
	coord_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	coord_buffer_info.pNext = nullptr;
	coord_buffer_info.flags = 0;
	coord_buffer_info.size = vert_coord.size() * sizeof(vert_coord[0]);
	coord_buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	coord_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	coord_buffer_info.queueFamilyIndexCount = 0;
	coord_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		logical_device,
		&coord_buffer_info,
		nullptr,
		&coord_buffer
	);
	if (vk_result != VK_SUCCESS) {
		std::cout << "Error: failed creating vertex buffer!" << std::endl;
	}

	// Allocating 
	VkMemoryRequirements coord_buffer_mem_req{};
	vkGetBufferMemoryRequirements(
		logical_device,
		coord_buffer,
		&coord_buffer_mem_req
	);

	VkDeviceMemory coord_buffer_memory = nullptr;
	VkMemoryAllocateInfo coord_buffer_memory_info{};
	coord_buffer_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	coord_buffer_memory_info.pNext = nullptr;
	coord_buffer_memory_info.allocationSize = coord_buffer_mem_req.size;
	coord_buffer_memory_info.memoryTypeIndex = 0;

	memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((coord_buffer_mem_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			coord_buffer_memory_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: vertex buffer memory type not found!");
	}

	vk_result = vkAllocateMemory(
		logical_device,
		&coord_buffer_memory_info,
		nullptr,
		&coord_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		std::cout << "Error: failed allocating vertex buffer memory!" << std::endl;
	}
	std::cout << "coord_buffer_mem_req.size: " << coord_buffer_mem_req.size << std::endl;
	// Maping value
	float* coord_copy;
	vk_result = vkMapMemory(
		logical_device,
		coord_buffer_memory,
		0,
		coord_buffer_mem_req.size,
		0,
		(void**)&coord_copy
	);
	memcpy(coord_copy, vert_coord.data(), coord_buffer_info.size);
	vkUnmapMemory(logical_device, coord_buffer_memory);

	// Binding
	vk_result = vkBindBufferMemory(
		logical_device,
		coord_buffer,
		coord_buffer_memory,
		0
	);
	if (vk_result != VK_SUCCESS) {
		std::cout << "Error: failed binding the vertex buffer memory!" << std::endl;
	}
	std::cout << "Coord buffer created." << std::endl;

	// Creating the	COLOR BUFFER
	VkBuffer color_buffer = nullptr;
	VkBufferCreateInfo color_buffer_info{};
	color_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	color_buffer_info.pNext = nullptr;
	color_buffer_info.flags = 0;
	color_buffer_info.size = vert_color.size() * sizeof(vert_color[0]);
	color_buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	color_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	color_buffer_info.queueFamilyIndexCount = 0;
	color_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		logical_device,
		&color_buffer_info,
		nullptr,
		&color_buffer
	);

	// Allocating memory
	VkMemoryRequirements color_buffer_mem_req{};
	vkGetBufferMemoryRequirements(
		logical_device,
		color_buffer,
		&color_buffer_mem_req
	);

	VkMemoryAllocateInfo color_buffer_memory_info{};
	color_buffer_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	color_buffer_memory_info.pNext = nullptr;
	color_buffer_memory_info.allocationSize = color_buffer_mem_req.size;
	color_buffer_memory_info.memoryTypeIndex = 0;

	memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((coord_buffer_mem_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			color_buffer_memory_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: color buffer memory type not found!");
	}

	VkDeviceMemory color_buffer_memory = nullptr;
	vk_result = vkAllocateMemory(
		logical_device,
		&color_buffer_memory_info,
		nullptr,
		&color_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating color buffer memory!");
	}

	// Mapping value
	float* color_copy;
	vk_result = vkMapMemory(
		logical_device,
		color_buffer_memory,
		0,
		color_buffer_mem_req.size,
		0,
		(void**)&color_copy
	);
	memcpy(color_copy, vert_color.data(), color_buffer_info.size);
	vkUnmapMemory(logical_device, color_buffer_memory);

	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed mapping color buffer values.");
	}

	// Binding
	vk_result = vkBindBufferMemory(
		logical_device,
		color_buffer,
		color_buffer_memory,
		0
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed binding color buffer values.");
	}

	std::cout << "Color buffer created." << std::endl;

	/**************************************************************/
	/* Step 13: Graphics Pipelines (10.2.)                        */
	/**************************************************************/
	//---- Vertex Input State (22.2)----//
	VkVertexInputBindingDescription coord_buff_input_binding_desc{};
	coord_buff_input_binding_desc.binding = 0; // Cette valeur doit être vu comme un identifiant, c'est pour faire le lien ensuite avec la structure VkVertexInnputAttributeDescription (attention, il n'y a pas de lien avec les valeurs dans le shader)
	coord_buff_input_binding_desc.stride = sizeof(vert_coord[0]);
	coord_buff_input_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputBindingDescription color_buff_input_binding_desc{};
	color_buff_input_binding_desc.binding = 1;
	color_buff_input_binding_desc.stride = sizeof(vert_color[0]);
	color_buff_input_binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputBindingDescription> vertex_input_binding_desc_arr = {
		coord_buff_input_binding_desc,
		color_buff_input_binding_desc
	};

	VkVertexInputAttributeDescription coord_input_desc{};
	coord_input_desc.location = 0;
	coord_input_desc.binding = coord_buff_input_binding_desc.binding;
	coord_input_desc.format = VK_FORMAT_R32G32B32_SFLOAT;
	coord_input_desc.offset = 0;

	VkVertexInputAttributeDescription color_input_desc{};
	color_input_desc.location = 1;
	color_input_desc.binding = color_buff_input_binding_desc.binding;
	color_input_desc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
	color_input_desc.offset = 0;

	std::vector<VkVertexInputAttributeDescription> vertex_input_attribute_arr{
		coord_input_desc,
		color_input_desc
	};

	VkPipelineVertexInputStateCreateInfo vertex_input_state_info{};
	vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_state_info.pNext = nullptr;
	vertex_input_state_info.flags = 0;
	vertex_input_state_info.vertexBindingDescriptionCount = vertex_input_binding_desc_arr.size();
	vertex_input_state_info.pVertexBindingDescriptions = vertex_input_binding_desc_arr.data();
	vertex_input_state_info.vertexAttributeDescriptionCount = vertex_input_attribute_arr.size();
	vertex_input_state_info.pVertexAttributeDescriptions = vertex_input_attribute_arr.data();

	//---- Input Assembly State (22.2.)----//
	VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info{};
	input_assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_state_info.pNext = nullptr;
	input_assembly_state_info.flags = 0;
	input_assembly_state_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_state_info.primitiveRestartEnable = VK_FALSE;

	//---- Viewport State (26.9.)----//
	std::vector<VkViewport> viewport_arr;
	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)swapchain_info.imageExtent.width;
	viewport.height = (float)swapchain_info.imageExtent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1.0;

	viewport_arr.push_back(viewport);

	std::vector<VkRect2D> scissor_arr;
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchain_info.imageExtent;

	scissor_arr.push_back(scissor);

	VkPipelineViewportStateCreateInfo viewport_state_info{};
	viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_info.pNext = nullptr;
	viewport_state_info.flags = 0;
	viewport_state_info.viewportCount = viewport_arr.size();
	viewport_state_info.pViewports = viewport_arr.data();
	viewport_state_info.scissorCount = scissor_arr.size();
	viewport_state_info.pScissors = scissor_arr.data();

	//---- Rasterization State (27.)----//
	VkPipelineRasterizationStateCreateInfo rasterization_state_info{};
	rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization_state_info.pNext = nullptr;
	rasterization_state_info.flags = 0;
	rasterization_state_info.depthClampEnable = VK_FALSE;
	rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
	rasterization_state_info.polygonMode = VK_POLYGON_MODE_FILL;
	rasterization_state_info.cullMode = VK_CULL_MODE_NONE;
	rasterization_state_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterization_state_info.depthBiasEnable = VK_FALSE;
	rasterization_state_info.depthBiasConstantFactor = 0.0;
	rasterization_state_info.depthBiasClamp = 0.0;
	rasterization_state_info.depthBiasSlopeFactor = 0.0;
	rasterization_state_info.lineWidth = 1.0;

	//---- Multisample State ()----//
	VkPipelineMultisampleStateCreateInfo multisample_state_info{};
	multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_state_info.pNext = nullptr;
	multisample_state_info.flags = 0;
	multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_state_info.sampleShadingEnable = VK_FALSE;
	multisample_state_info.minSampleShading = 0;
	multisample_state_info.pSampleMask = nullptr;
	multisample_state_info.alphaToCoverageEnable = VK_FALSE;
	multisample_state_info.alphaToOneEnable = VK_FALSE;

	//---- Depth Stencil State ()----//
	VkPipelineDepthStencilStateCreateInfo depth_stencil_state_info{};
	depth_stencil_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_state_info.pNext = nullptr;
	depth_stencil_state_info.flags = 0;
	depth_stencil_state_info.depthTestEnable = VK_TRUE;
	depth_stencil_state_info.depthWriteEnable = VK_TRUE;
	depth_stencil_state_info.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depth_stencil_state_info.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_state_info.stencilTestEnable = VK_FALSE;
	depth_stencil_state_info.front.failOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state_info.front.passOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state_info.front.compareOp = VK_COMPARE_OP_ALWAYS;
	depth_stencil_state_info.front.compareMask = 0;
	depth_stencil_state_info.front.reference = 0;
	depth_stencil_state_info.front.depthFailOp = VK_STENCIL_OP_KEEP;
	depth_stencil_state_info.front.writeMask = 0;
	depth_stencil_state_info.back = depth_stencil_state_info.front;
	depth_stencil_state_info.minDepthBounds = 0;
	depth_stencil_state_info.maxDepthBounds = 0;

	//---- Color Blend State ()----//
	std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_arr;
	VkPipelineColorBlendAttachmentState color_blend_attachment{};
	color_blend_attachment.blendEnable = VK_FALSE;
	color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
	color_blend_attachment.colorWriteMask = 0xf;

	color_blend_attachment_arr.push_back(color_blend_attachment);

	VkPipelineColorBlendStateCreateInfo color_blend_state_info{};
	color_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	color_blend_state_info.pNext = nullptr;
	color_blend_state_info.flags = 0;
	color_blend_state_info.logicOpEnable = VK_FALSE;
	color_blend_state_info.logicOp = VK_LOGIC_OP_CLEAR;
	color_blend_state_info.attachmentCount = color_blend_attachment_arr.size();
	color_blend_state_info.pAttachments = color_blend_attachment_arr.data();
	color_blend_state_info.blendConstants[0] = 1.0;
	color_blend_state_info.blendConstants[1] = 1.0;
	color_blend_state_info.blendConstants[2] = 1.0;
	color_blend_state_info.blendConstants[3] = 1.0;

	//---- Dynamic State (10.11.)----//
	std::vector<VkDynamicState> dynamic_state_arr = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamic_state_info{};
	dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state_info.pNext = nullptr;
	dynamic_state_info.flags = 0;
	dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(dynamic_state_arr.size());
	dynamic_state_info.pDynamicStates = dynamic_state_arr.data();

	//---- Creating the pipeline (10.2.)----//
	VkPipeline graphic_pipeline;
	VkGraphicsPipelineCreateInfo graphic_pipeline_info{};
	graphic_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphic_pipeline_info.pNext = nullptr;
	graphic_pipeline_info.flags = 0;
	graphic_pipeline_info.stageCount = shader_stage_arr.size();
	graphic_pipeline_info.pStages = shader_stage_arr.data();
	graphic_pipeline_info.pVertexInputState = &vertex_input_state_info;
	graphic_pipeline_info.pInputAssemblyState = &input_assembly_state_info;
	graphic_pipeline_info.pTessellationState = nullptr;
	graphic_pipeline_info.pViewportState = &viewport_state_info;
	graphic_pipeline_info.pRasterizationState = &rasterization_state_info;
	graphic_pipeline_info.pMultisampleState = &multisample_state_info;
	graphic_pipeline_info.pDepthStencilState = &depth_stencil_state_info;
	graphic_pipeline_info.pColorBlendState = &color_blend_state_info;
	graphic_pipeline_info.pDynamicState = &dynamic_state_info;
	graphic_pipeline_info.layout = pipeline_layout;
	graphic_pipeline_info.renderPass = render_pass;
	graphic_pipeline_info.subpass = 0;
	graphic_pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
	graphic_pipeline_info.basePipelineIndex = 0;

	vk_result = vkCreateGraphicsPipelines(
		logical_device,
		VK_NULL_HANDLE,
		1,
		&graphic_pipeline_info,
		nullptr,
		&graphic_pipeline
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed to create graphic pipeline!");
	}
	else {
		std::cout << "Graphic pipeline created." << std::endl;
	}

	/**************************************************************/
	/* Comp2: Compute Shader                                      */
	/**************************************************************/
	/**************************************************************/
	/* Comp 2.2: Shader Module (9.1)                              */
	/**************************************************************/
	std::vector<char> compute_shader_text = readFile("../shaders/compute.spv");
	VkShaderModule compute_shader_module = nullptr;
	VkShaderModuleCreateInfo compute_shader_module_info{};
	compute_shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	compute_shader_module_info.pNext = nullptr;
	compute_shader_module_info.flags = 0;
	compute_shader_module_info.codeSize = compute_shader_text.size();
	compute_shader_module_info.pCode = reinterpret_cast<const uint32_t*>(compute_shader_text.data());

	vk_result = vkCreateShaderModule(
		logical_device,
		&compute_shader_module_info,
		nullptr,
		&compute_shader_module
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating compute shader module!");
	}
	else {
		std::cout << "Compute shader module created." << std::endl;
	}

	VkPipelineShaderStageCreateInfo compute_shader_stage{};
	compute_shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	compute_shader_stage.pNext = nullptr;
	compute_shader_stage.flags = 0;
	compute_shader_stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	compute_shader_stage.module = compute_shader_module;
	compute_shader_stage.pName = "main";
	compute_shader_stage.pSpecializationInfo = nullptr;

	std::cout << "Compute Shader created." << std::endl;

	/**************************************************************/
	/* Comp 3: Storage Buffer                                     */
	/**************************************************************/
	// --- data --- //
	std::vector<int> input_data(4, 2);
	std::vector<int> output_data(input_data.size(), -1);

	// --- Storage Buffer --- //
	// -- INPUT DATA -- //
	VkBuffer input_data_buffer = nullptr;
	VkBufferCreateInfo input_data_buffer_info{};
	input_data_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	input_data_buffer_info.pNext = nullptr;
	input_data_buffer_info.flags = 0;
	input_data_buffer_info.size = input_data.size() * sizeof(input_data[0]);
	input_data_buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	input_data_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	input_data_buffer_info.queueFamilyIndexCount = 0;
	input_data_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		logical_device,
		&input_data_buffer_info,
		nullptr,
		&input_data_buffer
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating storage buffer.");
	}

	VkMemoryRequirements input_data_buffer_memory_req{};
	vkGetBufferMemoryRequirements(
		logical_device,
		input_data_buffer,
		&input_data_buffer_memory_req
	);

	VkDeviceMemory input_data_buffer_memory = nullptr;
	VkMemoryAllocateInfo input_data_buffer_memory_info{};
	input_data_buffer_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	input_data_buffer_memory_info.pNext = nullptr;
	input_data_buffer_memory_info.allocationSize = input_data_buffer_memory_req.size;
	input_data_buffer_memory_info.memoryTypeIndex = 0;

	memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((input_data_buffer_memory_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			input_data_buffer_memory_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: uniform buffer memory type not found!");
	}

	vk_result = vkAllocateMemory(
		logical_device,
		&input_data_buffer_memory_info,
		nullptr,
		&input_data_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating input data storage buffer memory!");
	}

	int* input_data_copy;
	vk_result = vkMapMemory(
		logical_device,
		input_data_buffer_memory,
		0,
		input_data_buffer_memory_req.size,
		0,
		(void**)&input_data_copy
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: maping input data buffer memory failed!");
	}

	memcpy(input_data_copy, input_data.data(), input_data_buffer_memory_req.size);
	vkUnmapMemory(logical_device, input_data_buffer_memory);
	vk_result = vkBindBufferMemory(
		logical_device,
		input_data_buffer,
		input_data_buffer_memory,
		0
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: input data buffer binding failed!");
	}

	std::cout << "Input data buffer created." << std::endl;

	// -- OUTPUT DATA -- //
	VkBuffer output_data_buffer = nullptr;
	VkBufferCreateInfo output_data_buffer_info{};
	output_data_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	output_data_buffer_info.pNext = nullptr;
	output_data_buffer_info.flags = 0;
	output_data_buffer_info.size = output_data.size() * sizeof(output_data[0]);
	output_data_buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	output_data_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	output_data_buffer_info.queueFamilyIndexCount = 0;
	output_data_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		logical_device,
		&output_data_buffer_info,
		nullptr,
		&output_data_buffer
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating storage buffer.");
	}

	VkMemoryRequirements output_data_buffer_memory_req{};
	vkGetBufferMemoryRequirements(
		logical_device,
		output_data_buffer,
		&output_data_buffer_memory_req
	);

	VkDeviceMemory output_data_buffer_memory = nullptr;
	VkMemoryAllocateInfo output_data_buffer_memory_info{};
	output_data_buffer_memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	output_data_buffer_memory_info.pNext = nullptr;
	output_data_buffer_memory_info.allocationSize = output_data_buffer_memory_req.size;
	output_data_buffer_memory_info.memoryTypeIndex = 0;

	memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (memory_type_i = 0; memory_type_i < memory_properties.memoryTypeCount; memory_type_i++) {
		if ((output_data_buffer_memory_req.memoryTypeBits & (1 << memory_type_i)) && (memory_properties.memoryTypes[memory_type_i].propertyFlags & memory_property_flags) == memory_property_flags) {
			output_data_buffer_memory_info.memoryTypeIndex = memory_type_i;
			break;
		}
	}
	if (memory_type_i > memory_properties.memoryTypeCount) {
		throw std::runtime_error("Error: uniform buffer memory type not found!");
	}

	vk_result = vkAllocateMemory(
		logical_device,
		&output_data_buffer_memory_info,
		nullptr,
		&output_data_buffer_memory
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed allocating output data storage buffer memory!");
	}

	int* output_data_copy;
	vk_result = vkMapMemory(
		logical_device,
		output_data_buffer_memory,
		0,
		output_data_buffer_memory_req.size,
		0,
		(void**)&output_data_copy
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: maping output data buffer memory failed!");
	}

	memcpy(output_data_copy, output_data.data(), output_data_buffer_memory_req.size);
	vkUnmapMemory(logical_device, output_data_buffer_memory);
	vk_result = vkBindBufferMemory(
		logical_device,
		output_data_buffer,
		output_data_buffer_memory,
		0
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: output data buffer binding failed!");
	}

	std::cout << "Output data buffer created." << std::endl;

	/**************************************************************/
	/* Step 14: Fences and Semaphores (7.3. and 7.4.)             */
	/**************************************************************/
	/**************************************************************/
	/* Step 14.1: Fences (7.3.)                                   */
	/**************************************************************/
	std::vector<VkFence> in_flight_fence_arr(framebuffer_arr.size());
	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t fence_i = 0; fence_i < in_flight_fence_arr.size(); fence_i++) {
		vk_result = vkCreateFence(
			logical_device,
			&fence_info,
			nullptr,
			&in_flight_fence_arr[fence_i]
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed creating fence!");
		}
		else {
			std::cout << "Fence created." << std::endl;
		}
	}

	/**************************************************************/
	/* Step 14.2: Semaphores (7.4.)                               */
	/**************************************************************/
	std::vector<VkSemaphore> image_available_sem_arr(framebuffer_arr.size());
	std::vector<VkSemaphore> render_finished_sem_arr(framebuffer_arr.size());
	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_info.pNext = nullptr;
	semaphore_info.flags = 0;

	for (size_t sem_i = 0; sem_i < image_available_sem_arr.size(); sem_i++) {
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

	/**************************************************************/
	/* Step 15: Drawing (21.3. and 33.10.)                        */
	/**************************************************************/
	/********************/
	/* Game Loop        */
	/********************/
	uint32_t current_frame{ 0 };
	while (!glfwWindowShouldClose(window)) {
		// Get available image (to draw into)
		uint32_t image_index{ 0 };

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

		vk_result = vkAcquireNextImageKHR(
			logical_device,
			swapchain,
			UINT64_MAX,
			image_available_sem_arr[current_frame],
			VK_NULL_HANDLE,
			&image_index
		);
		if (vk_result != VK_SUCCESS) {
			throw std::runtime_error("Error: failed acquiring next image!");
		}

		/*********************/
		/* Bindings for draw */
		/*********************/
		vkResetCommandBuffer(
			command_buffer_arr[current_frame],
			0
		);
		//---- Command Buffer Recording (6.4.)----//
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
		std::vector<VkSemaphore> wait_semaphore_arr = { image_available_sem_arr[current_frame] };
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

		current_frame = (current_frame + 1) % nb_frame;

		glfwPollEvents();
	}

	// To wait for every fences and semaphores to be released
	vkDeviceWaitIdle(logical_device);

	/**************/
	/* Destroying */
	/**************/
	// Semaphore
	for (auto sem : render_finished_sem_arr) {
		vkDestroySemaphore(logical_device, sem, nullptr);
	}
	for (auto sem : image_available_sem_arr) {
		vkDestroySemaphore(logical_device, sem, nullptr);
	}
	// Fence
	for (auto f : in_flight_fence_arr) {
		vkDestroyFence(logical_device, f, nullptr);
	}
	// Pipeline
	vkDestroyPipeline(logical_device, graphic_pipeline, nullptr);
	// Color buffer
	vkFreeMemory(logical_device, color_buffer_memory, nullptr);
	vkDestroyBuffer(logical_device, color_buffer, nullptr);
	// Vertex buffer
	vkFreeMemory(logical_device, coord_buffer_memory, nullptr);
	vkDestroyBuffer(logical_device, coord_buffer, nullptr);
	// Framebuffer
	for (auto fb : framebuffer_arr) {
		vkDestroyFramebuffer(logical_device, fb, nullptr);
	}
	// Shader Module
	vkDestroyShaderModule(logical_device, fragment_shader_module, nullptr);
	vkDestroyShaderModule(logical_device, vertex_shader_module, nullptr);
	vkDestroyShaderModule(logical_device, compute_shader_module, nullptr);
	// Compute buffers
	// Ouptut
	vkFreeMemory(logical_device, output_data_buffer_memory, nullptr);
	vkDestroyBuffer(logical_device, output_data_buffer, nullptr);
	// Input
	vkFreeMemory(logical_device, input_data_buffer_memory, nullptr);
	vkDestroyBuffer(logical_device, input_data_buffer, nullptr);
	// Render Pass
	vkDestroyRenderPass(logical_device, render_pass, nullptr);
	// Descriptor sets
	for (int i = 0; i < descriptor_sets_arr.size(); i++) {
		vk_result = vkFreeDescriptorSets(
			logical_device,
			descriptor_pool,
			descriptor_sets_info_arr[i].descriptorSetCount,
			&descriptor_sets_arr[i]
		);
	}
	// Descriptor pool
	vkDestroyDescriptorPool(logical_device, descriptor_pool, nullptr);
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
}