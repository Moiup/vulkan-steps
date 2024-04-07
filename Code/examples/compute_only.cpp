#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

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

void resultChecker(const VkResult vk_result, const std::string msg) {
	if (vk_result != VK_SUCCESS) {
		std::cerr << msg << string_VkResult(vk_result) << std::endl;
		exit(EXIT_FAILURE);
	}
}

int main() {
	VkResult vk_result{};

	/**************************************************************/
	/* Instances                                                  */
	/**************************************************************/

	uint32_t nb_layers{};
	std::vector<VkLayerProperties> layers{};

	// Wanted layers
	std::vector<const char*> layers_name{
		"VK_LAYER_KHRONOS_validation"
	};

	vk_result = vkEnumerateInstanceLayerProperties(&nb_layers, nullptr);
	resultChecker(vk_result, "Error getting layer properties count: ");
	layers.resize(nb_layers);
	vk_result = vkEnumerateInstanceLayerProperties(&nb_layers, layers.data());
	resultChecker(vk_result, "Error getting layer properties: ");
	std::cout << "Available layer properties:" << std::endl;
	for (VkLayerProperties& prop : layers) {
		std::cout << "\t" << prop.layerName << " - " << prop.specVersion << " - " << prop.description << std::endl;
	}
	std::cout << std::endl;

	// Looking for wanted layers
	bool layer_found{ false };
	for (const char* l : layers_name) {
		for (VkLayerProperties& prop : layers) {
			if (strcmp(prop.layerName, l) == 0) {
				layer_found = true;
				break;
			}
			layer_found = false;
		}
	}

	if (!layer_found) {
		std::cerr << "Error: not all layers were found." << std::endl;
		return EXIT_FAILURE;
	}

	VkApplicationInfo app_info{};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = nullptr;
	app_info.pApplicationName = "Surfaceless compute shader";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Compute Demo Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_3;

	VkInstance instance{};
	VkInstanceCreateInfo instance_info{};
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pNext = nullptr;
	instance_info.flags = 0;
	instance_info.pApplicationInfo = &app_info;
	instance_info.enabledLayerCount = layers_name.size();
	instance_info.ppEnabledLayerNames = layers_name.data();
	instance_info.enabledExtensionCount = 0;
	instance_info.ppEnabledExtensionNames = nullptr;

	vk_result = vkCreateInstance(
		&instance_info,
		nullptr,
		&instance
	);
	resultChecker(vk_result, "Error creating instance : ");

	/**************************************************************/
	/* Physical device                                            */
	/**************************************************************/

	uint32_t nb_physical_devices{};
	std::vector<VkPhysicalDevice> physical_devices;
	vk_result = vkEnumeratePhysicalDevices(
		instance,
		&nb_physical_devices,
		nullptr
	);
	resultChecker(vk_result, "Error retreiving physical device count: ");
	physical_devices.resize(nb_physical_devices);
	vk_result = vkEnumeratePhysicalDevices(
		instance,
		&nb_physical_devices,
		physical_devices.data()
	);
	resultChecker(vk_result, "Error retreiving physical device: ");

	VkPhysicalDevice gpu{nullptr};
	std::cout << "Available physical devices" << std::endl;
	for (VkPhysicalDevice& physical_device : physical_devices) {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(
			physical_device,
			&properties
		);

		std::cout << "\t" << properties.deviceName << std::endl;
		std::cout << "\t" << properties.driverVersion << std::endl;
		std::cout << "\t" << string_VkPhysicalDeviceType(properties.deviceType) << std::endl;
		std::cout << "\t" << properties.apiVersion << std::endl;
		std::cout << "\t" << "-----------------------------------------" << std::endl;

		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			gpu = physical_device;
		}
		else if (gpu == nullptr) {
			gpu = physical_device;
		}
	}
	std::cout << std::endl;

	if (!gpu) {
		std::cerr << "Error: no physical device found!" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "Selected GPU:" << std::endl;
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(
			gpu,
			&properties
		);

		std::cout << "\t" << properties.deviceName << std::endl;
		std::cout << "\t" << properties.driverVersion << std::endl;
		std::cout << "\t" << string_VkPhysicalDeviceType(properties.deviceType) << std::endl;
		std::cout << "\t" << properties.apiVersion << std::endl;
	}
	std::cout << std::endl;

	/**************************************************************/
	/* Queue properties                                           */
	/**************************************************************/
	VkQueue compute_queue;

	std::vector<float> queue_priorities{ 1.0f };
	VkDeviceQueueCreateInfo queue_info{};
	queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.pNext = nullptr;
	queue_info.flags = 0;
	queue_info.queueFamilyIndex = 0;
	queue_info.queueCount = 1;
	queue_info.pQueuePriorities = queue_priorities.data();

	uint32_t nb_queue_properties;
	std::vector<VkQueueFamilyProperties> queue_properties;
	vkGetPhysicalDeviceQueueFamilyProperties(
		gpu,
		&nb_queue_properties,
		nullptr
	);
	queue_properties.resize(nb_queue_properties);
	vkGetPhysicalDeviceQueueFamilyProperties(
		gpu,
		&nb_queue_properties,
		queue_properties.data()
	);

	bool queue_found{ false };
	for (VkQueueFamilyProperties& prop : queue_properties) {
		if (prop.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			queue_found = true;
			break;
		}
		queue_info.queueFamilyIndex++;
	}

	if (!queue_found) {
		std::cerr << "Error: no proper queue found!" << std::endl;
		exit(EXIT_FAILURE);
	}

	/**************************************************************/
	/* Device                                                     */
	/**************************************************************/
	VkDevice device{};
	VkDeviceCreateInfo device_info{};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = nullptr;
	device_info.flags = 0;
	device_info.queueCreateInfoCount = 1;
	device_info.pQueueCreateInfos = &queue_info;
	device_info.enabledLayerCount = layers_name.size();
	device_info.ppEnabledLayerNames = layers_name.data();
	device_info.enabledExtensionCount = 0;
	device_info.ppEnabledExtensionNames = nullptr;
	device_info.pEnabledFeatures = nullptr;

	vk_result = vkCreateDevice(
		gpu,
		&device_info,
		nullptr,
		&device
	);
	resultChecker(vk_result, "Error creating device: ");


	// Actually retreiving the queue
	vkGetDeviceQueue(
		device,
		queue_info.queueFamilyIndex,
		0,
		&compute_queue
	);

	/**************************************************************/
	/* Command Pool                                               */
	/**************************************************************/
	VkCommandPool command_pool{};
	VkCommandPoolCreateInfo cmd_pool_info{};
	cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmd_pool_info.pNext = nullptr;
	cmd_pool_info.flags = 0;
	cmd_pool_info.queueFamilyIndex = queue_info.queueFamilyIndex;

	vk_result = vkCreateCommandPool(
		device,
		&cmd_pool_info,
		nullptr,
		&command_pool
	);
	resultChecker(vk_result, "Error creating command pool: ");

	/**************************************************************/
	/* Storage buffer                                             */
	/**************************************************************/
	// ------------------ Memory Properties ------------------------
	VkPhysicalDeviceMemoryProperties memory_properties{};
	vkGetPhysicalDeviceMemoryProperties(gpu, &memory_properties);


	// For ease of use, will create staging buffers
	// (host and device visible buffers)
	// ---------------------- BUFFER 1 -----------------------------
	std::vector<int32_t> input_data(10, 10);
	VkBuffer input_buffer{};
	VkBufferCreateInfo input_buffer_info;
	input_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	input_buffer_info.pNext = nullptr;
	input_buffer_info.flags = 0;
	input_buffer_info.size = static_cast<uint32_t>(input_data.size() * sizeof(int32_t));
	input_buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	input_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	input_buffer_info.queueFamilyIndexCount = queue_info.queueFamilyIndex;
	input_buffer_info.pQueueFamilyIndices = nullptr;

	vk_result = vkCreateBuffer(
		device,
		&input_buffer_info,
		nullptr,
		&input_buffer
	);
	resultChecker(vk_result, "Error creating input buffer: ");

	VkDeviceMemory input_buffer_memory;
	{
		VkMemoryRequirements memory_requirements{};
		vkGetBufferMemoryRequirements(
			device,
			input_buffer,
			&memory_requirements
		);

		VkMemoryAllocateInfo memory_allocate_info{};
		memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memory_allocate_info.pNext = nullptr;
		memory_allocate_info.allocationSize = memory_requirements.size;
		memory_allocate_info.memoryTypeIndex;

		bool is_found{ false };
		VkMemoryPropertyFlags desired_type = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		for (uint32_t i = 0; i < (memory_properties.memoryTypeCount) && !is_found; ++i) {
			is_found = (memory_requirements.memoryTypeBits & desired_type) & (memory_properties.memoryTypes[i].propertyFlags);
			memory_allocate_info.memoryTypeIndex = i;
		}
		if (!is_found) {
			std::cerr << "Error: the required memory properties were not found!" << std::endl;
			return EXIT_FAILURE;
		}

		vk_result = vkAllocateMemory(
			device,
			&memory_allocate_info,
			nullptr,
			&input_buffer_memory
		);
	};

	vk_result = vkBindBufferMemory(
		device,
		input_buffer,
		input_buffer_memory,
		0
	);

	// Assigning the value to the buffer
	{
		void* data;
		vkMapMemory(
			device,
			input_buffer_memory,
			0,
			input_buffer_info.size,
			0,
			&data
		);
		memcpy(data, input_data.data(), input_buffer_info.size);
		vkUnmapMemory(device, input_buffer_memory);
	}

	// ---------------------- BUFFER 2 -----------------------------
	std::vector<int32_t> output_data(10, 0);
	VkBuffer output_buffer{};
	VkBufferCreateInfo output_buffer_info;
	output_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	output_buffer_info.pNext = nullptr;
	output_buffer_info.flags = 0;
	output_buffer_info.size = output_data.size() * sizeof(uint32_t);
	output_buffer_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	output_buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	output_buffer_info.queueFamilyIndexCount = queue_info.queueFamilyIndex;
	output_buffer_info.pQueueFamilyIndices = nullptr;
	
	vk_result = vkCreateBuffer(
		device,
		&output_buffer_info,
		nullptr,
		&output_buffer
	);
	resultChecker(vk_result, "Error creating output buffer: ");
	
	VkDeviceMemory output_buffer_memory;
	{
		VkMemoryRequirements memory_requirements{};
		vkGetBufferMemoryRequirements(
			device,
			output_buffer,
			&memory_requirements
		);

		VkMemoryAllocateInfo memory_allocate_info{};
		memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memory_allocate_info.pNext = nullptr;
		memory_allocate_info.allocationSize = memory_requirements.size;
		memory_allocate_info.memoryTypeIndex;

		bool is_found{ false };
		VkMemoryPropertyFlags desired_type = (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		for (uint32_t i = 0; i < (memory_properties.memoryTypeCount) && !is_found; ++i) {
			is_found = (memory_requirements.memoryTypeBits & desired_type) & (memory_properties.memoryTypes[i].propertyFlags);
			memory_allocate_info.memoryTypeIndex = i;
		}
		if (!is_found) {
			std::cerr << "Error: the required memory properties were not found!" << std::endl;
			return EXIT_FAILURE;
		}

		vk_result = vkAllocateMemory(
			device,
			&memory_allocate_info,
			nullptr,
			&output_buffer_memory
		);
	};
	vk_result = vkBindBufferMemory(
		device,
		output_buffer,
		output_buffer_memory,
		0
	);

	/**************************************************************/
	/* Descriptor Set Layout                                      */
	/**************************************************************/
	std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings{};

	{
		// inputData binding
		VkDescriptorSetLayoutBinding descriptor_set_layout_binding{};
		descriptor_set_layout_binding.binding = 0;
		descriptor_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptor_set_layout_binding.descriptorCount = 1;
		descriptor_set_layout_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		descriptor_set_layout_binding.pImmutableSamplers = nullptr;
		descriptor_set_layout_bindings.push_back(descriptor_set_layout_binding);
	}
	{
		// outputData binding
		VkDescriptorSetLayoutBinding descriptor_set_layout_binding{};
		descriptor_set_layout_binding.binding = 1;
		descriptor_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptor_set_layout_binding.descriptorCount = 1;
		descriptor_set_layout_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		descriptor_set_layout_binding.pImmutableSamplers = nullptr;
		descriptor_set_layout_bindings.push_back(descriptor_set_layout_binding);
	}

	VkDescriptorSetLayout descriptor_set_layout{};
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
	descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_info.pNext = nullptr;
	descriptor_set_layout_info.flags = 0;
	descriptor_set_layout_info.bindingCount = descriptor_set_layout_bindings.size();
	descriptor_set_layout_info.pBindings = descriptor_set_layout_bindings.data();

	vk_result = vkCreateDescriptorSetLayout(
		device,
		&descriptor_set_layout_info,
		nullptr,
		&descriptor_set_layout
	);
	resultChecker(vk_result, "Error creating descriptor set layout: ");

	/**************************************************************/
	/* Descriptor Pool                                            */
	/**************************************************************/
	std::vector<VkDescriptorPoolSize> descriptor_pool_sizes;
	for(VkDescriptorSetLayoutBinding& dslb : descriptor_set_layout_bindings){
		VkDescriptorPoolSize pool_size{};
		pool_size.type = dslb.descriptorType;
		pool_size.descriptorCount = dslb.descriptorCount;
		descriptor_pool_sizes.push_back(pool_size);
	}

	VkDescriptorPool descriptor_pool{};
	VkDescriptorPoolCreateInfo descriptor_pool_info{};
	descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_info.pNext = nullptr;
	descriptor_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	descriptor_pool_info.maxSets = 1;
	descriptor_pool_info.poolSizeCount = descriptor_pool_sizes.size();
	descriptor_pool_info.pPoolSizes = descriptor_pool_sizes.data();
	vk_result = vkCreateDescriptorPool(
		device,
		&descriptor_pool_info,
		nullptr,
		&descriptor_pool
	);
	resultChecker(vk_result, "Error creating descriptor pool: ");

	/**************************************************************/
	/* Descriptor Set                                             */
	/**************************************************************/
	VkDescriptorSet descriptor_set{};
	VkDescriptorSetAllocateInfo descriptor_set_info{};
	descriptor_set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_info.pNext = nullptr;
	descriptor_set_info.descriptorPool = descriptor_pool;
	descriptor_set_info.descriptorSetCount = 1;
	descriptor_set_info.pSetLayouts = &descriptor_set_layout;

	vk_result = vkAllocateDescriptorSets(
		device,
		&descriptor_set_info,
		&descriptor_set
	);
	resultChecker(vk_result, "Error allocating descriptor set: ");

	/**************************************************************/
	/* Update Descriptor Sets                                     */
	/**************************************************************/
	std::vector<VkWriteDescriptorSet> write_descriptor_sets{};
	{
		VkDescriptorSetLayoutBinding& dslb = descriptor_set_layout_bindings[0];

		VkDescriptorBufferInfo descriptor_buffer_info{};
		descriptor_buffer_info.buffer = input_buffer;
		descriptor_buffer_info.offset = 0;
		descriptor_buffer_info.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet write_descriptor_set{};
		write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set.pNext = nullptr;
		write_descriptor_set.dstSet = descriptor_set;
		write_descriptor_set.dstBinding = dslb.binding;
		write_descriptor_set.dstArrayElement = 0;
		write_descriptor_set.descriptorCount = dslb.descriptorCount;
		write_descriptor_set.descriptorType = dslb.descriptorType;
		write_descriptor_set.pImageInfo = nullptr;
		write_descriptor_set.pBufferInfo = &descriptor_buffer_info;
		write_descriptor_set.pTexelBufferView = nullptr;

		write_descriptor_sets.push_back(write_descriptor_set);
	};
	{
		VkDescriptorSetLayoutBinding& dslb = descriptor_set_layout_bindings[1];

		VkDescriptorBufferInfo descriptor_buffer_info{};
		descriptor_buffer_info.buffer = output_buffer;
		descriptor_buffer_info.offset = 0;
		descriptor_buffer_info.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet write_descriptor_set{};
		write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set.pNext = nullptr;
		write_descriptor_set.dstSet = descriptor_set;
		write_descriptor_set.dstBinding = dslb.binding;
		write_descriptor_set.dstArrayElement = 0;
		write_descriptor_set.descriptorCount = dslb.descriptorCount;
		write_descriptor_set.descriptorType = dslb.descriptorType;
		write_descriptor_set.pImageInfo = nullptr;
		write_descriptor_set.pBufferInfo = &descriptor_buffer_info;
		write_descriptor_set.pTexelBufferView = nullptr;

		write_descriptor_sets.push_back(write_descriptor_set);
	};

	vkUpdateDescriptorSets(
		device,
		write_descriptor_sets.size(),
		write_descriptor_sets.data(),
		0,
		nullptr
	);

	/**************************************************************/
	/* Pipeline Layout                                            */
	/**************************************************************/
	VkPipelineLayout pipeline_layout{};
	VkPipelineLayoutCreateInfo pipeline_layout_info{};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.pNext = nullptr;
	pipeline_layout_info.flags = 0;
	pipeline_layout_info.setLayoutCount = 1;
	pipeline_layout_info.pSetLayouts = &descriptor_set_layout;
	pipeline_layout_info.pushConstantRangeCount = 0;
	pipeline_layout_info.pPushConstantRanges = nullptr;


	vk_result = vkCreatePipelineLayout(
		device,
		&pipeline_layout_info,
		nullptr,
		&pipeline_layout
	);
	resultChecker(vk_result, "Error creating pipeline layout: ");

	/**************************************************************/
	/* Shader                                                     */
	/**************************************************************/
	const std::vector<char>&& shader_code = readFile("../shaders/compute.spv");

	VkShaderModule shader_module{};
	VkShaderModuleCreateInfo shader_module_info{};
	shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shader_module_info.pNext = nullptr;
	shader_module_info.flags = 0;
	shader_module_info.codeSize = shader_code.size();
	shader_module_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());

	vk_result = vkCreateShaderModule(
		device,
		&shader_module_info,
		nullptr,
		&shader_module
	);
	resultChecker(vk_result, "Error creating shader module: ");

	/**************************************************************/
	/* Compute Pipeline                                           */
	/**************************************************************/
	VkPipelineShaderStageCreateInfo shader_stage_info{};
	shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_info.pNext = nullptr;
	shader_stage_info.flags = 0;
	shader_stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	shader_stage_info.module = shader_module;
	shader_stage_info.pName = "main";
	shader_stage_info.pSpecializationInfo = nullptr;

	VkPipeline pipeline{};
	VkComputePipelineCreateInfo pipeline_info{};
	pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipeline_info.pNext = nullptr;
	pipeline_info.flags = 0;
	pipeline_info.stage = shader_stage_info;
	pipeline_info.layout = pipeline_layout;
	pipeline_info.basePipelineHandle = nullptr;
	pipeline_info.basePipelineIndex = 0;

	vk_result = vkCreateComputePipelines(
		device,
		nullptr,
		1,
		&pipeline_info,
		nullptr,
		&pipeline
	);

	/**************************************************************/
	/* Command Buffer                                             */
	/**************************************************************/
	VkCommandBuffer command_buffer{};
	VkCommandBufferAllocateInfo command_buffer_info{};
	command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_info.pNext = nullptr;
	command_buffer_info.commandPool = command_pool;
	command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_info.commandBufferCount = 1;

	vk_result = vkAllocateCommandBuffers(
		device,
		&command_buffer_info,
		&command_buffer
	);
	resultChecker(vk_result, "Error allocating command buffer: ");

	/**************************************************************/
	/* Fence                                                      */
	/**************************************************************/
	VkFence fence{};
	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.pNext = nullptr;
	fence_info.flags = 0;

	vk_result = vkCreateFence(
		device,
		&fence_info,
		nullptr,
		&fence
	);
	resultChecker(vk_result, "Error creating fence: ");

	/**************************************************************/
	/* Dispatch                                                   */
	/**************************************************************/
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.pNext = nullptr;
	begin_info.flags = 0;
	begin_info.pInheritanceInfo = nullptr;
	vkBeginCommandBuffer(command_buffer, &begin_info);

	vkCmdBindDescriptorSets(
		command_buffer,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		pipeline_layout,
		0,
		1,
		&descriptor_set,
		0,
		nullptr
	);

	vkCmdBindPipeline(
		command_buffer,
		VK_PIPELINE_BIND_POINT_COMPUTE,
		pipeline
	);

	vkCmdDispatch(
		command_buffer,
		input_data.size(),
		1,
		1
	);

	vkEndCommandBuffer(command_buffer);

	/**************************************************************/
	/* Step 1: Submit (synchronised)                              */
	/**************************************************************/
	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = nullptr;
	submit_info.waitSemaphoreCount = 0;
	submit_info.pWaitSemaphores = nullptr;
	submit_info.pWaitDstStageMask = nullptr;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores = nullptr;

	vkQueueSubmit(
		compute_queue,
		1,
		&submit_info,
		fence
	);

	vkWaitForFences(
		device,
		1,
		&fence,
		VK_TRUE,
		UINT64_MAX
	);

	// Displaying the output buffer value
	{
		void* data;
		vkMapMemory(
			device,
			output_buffer_memory,
			0,
			output_buffer_info.size,
			0,
			&data
		);
		std::cout << "Displaying output data:" << std::endl;
		uint32_t* output = (uint32_t*)(data);
		for (uint32_t i = 0; i < output_data.size(); ++i) {
			std::cout << "\t" << output[i] << std::endl;
			output_data[i] = output[i];
		}

		vkUnmapMemory(device, output_buffer_memory);
	}

	/**************************************************************/
	/* Destroy / Free                                             */
	/**************************************************************/

	vkDestroyFence(device, fence, nullptr);

	vkFreeMemory(device, output_buffer_memory, nullptr);
	vkDestroyBuffer(device, output_buffer, nullptr);
	vkFreeMemory(device, input_buffer_memory, nullptr);
	vkDestroyBuffer(device, input_buffer, nullptr);

	vk_result = vkFreeDescriptorSets(
		device,
		descriptor_pool,
		1,
		&descriptor_set
	);
	resultChecker(vk_result, "Error freeing descriptor set: ");
	vkDestroyDescriptorPool(device, descriptor_pool, nullptr);

	vkDestroyDescriptorSetLayout(device, descriptor_set_layout, nullptr);
	vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
	vkDestroyShaderModule(device, shader_module, nullptr);
	vkDestroyPipeline(device, pipeline, nullptr);

	vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
	vkDestroyCommandPool(device, command_pool, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroyInstance(instance, nullptr);

	return EXIT_SUCCESS;
}