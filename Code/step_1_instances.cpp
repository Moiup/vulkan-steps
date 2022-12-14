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

	/********************/
	/* Game Loop        */
	/********************/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}



	/**************/
	/* Destroying */
	/**************/
	// Instance
	vkDestroyInstance(vk_instance, nullptr);

	glfwTerminate();
	std::cout << "Programme terminated successfully" << std::endl;
}
