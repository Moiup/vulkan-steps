# **Comp 4.1: Descriptor Set Layout (14.2.1.)**
## **Hints**
Descriptor Set Layout are created using [`vkCreateDescriptorSetLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkCreateDescriptorSetLayout).

## **The Code**
When creating Descriptor Set Layout, we are in fact creating an array of [`VkDescriptorSetLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#descriptorsets-setlayout). We create only one, because bothe the input and output belongs to the same set.

In parallel, require an array of two [`VkDescriptorSetLayoutBinding`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorSetLayoutBinding) (one for the input and one for the output.) to be affected inside the [`VkDescriptorSetLayoutCreateInfo`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorSetLayoutCreateInfo) structure.

Then we can call [`vkCreateDescriptorSetLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkCreateDescriptorSetLayout).

In [`VkDescriptorSetLayoutBinding`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#VkDescriptorSetLayoutBinding) the `binding` field must take the input and output storage buffer binding value in the Compute Shader, in our case they are 0 and 1.

```C
// UNIFORM BUFFER
layout (std430, binding = 0) readonly buffer inputData
{
    int input_data[];
};

layout (std430, binding = 1) writeonly buffer outputData
{
    int output_data[];
};
```

We need to destroy each Descriptor Set Layout with [`vkDestroyDescriptorSetLayout`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap14.html#vkDestroyDescriptorSetLayout).

```C++
    /**************************************************************/
	/* Comp 4: Descriptor Sets (14.2.)                            */
	/**************************************************************/
	/**************************************************************/
	/* Comp 4.1: Descriptor Set Layout (14.2.1)                   */
	/**************************************************************/
    // Input
	VkDescriptorSetLayoutBinding input_descriptor_set_binding{};
	input_descriptor_set_binding.binding = 0;
	input_descriptor_set_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	input_descriptor_set_binding.descriptorCount = 1;
	input_descriptor_set_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	input_descriptor_set_binding.pImmutableSamplers = nullptr;

	// Output
	VkDescriptorSetLayoutBinding output_descriptor_set_binding{};
	output_descriptor_set_binding.binding = 1;
	output_descriptor_set_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	output_descriptor_set_binding.descriptorCount = 1;
	output_descriptor_set_binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	output_descriptor_set_binding.pImmutableSamplers = nullptr;

	std::vector<VkDescriptorSetLayoutBinding> compute_descriptor_set_binding_arr = {
		input_descriptor_set_binding,
		output_descriptor_set_binding
	};

	uint32_t nb_compute_descriptor_set = compute_descriptor_set_binding_arr.size();

	VkDescriptorSetLayout compute_descriptor_set_layout = nullptr;
	VkDescriptorSetLayoutCreateInfo compute_descriptor_set_layout_info{};
	compute_descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	compute_descriptor_set_layout_info.pNext = nullptr;
	compute_descriptor_set_layout_info.flags = 0;
	compute_descriptor_set_layout_info.bindingCount = compute_descriptor_set_binding_arr.size();
	compute_descriptor_set_layout_info.pBindings = compute_descriptor_set_binding_arr.data();

	vk_result = vkCreateDescriptorSetLayout(
		logical_device,
		&compute_descriptor_set_layout_info,
		nullptr,
		&compute_descriptor_set_layout
	);
	if (vk_result != VK_SUCCESS) {
		throw std::runtime_error("Error: failed creating the compute descriptor set layout!");
	}

	std::vector<VkDescriptorSetLayout> compute_descriptor_set_layout_arr(1, compute_descriptor_set_layout);
```

File: [comp4_1_descriptor_set_layout.cpp](../../Code/comp_4_1_descriptor_set_layout.cpp)

| Previous | Next |
|---|---|
| [Comp 4: Descriptor Sets (14.2.)](comp4_descriptor_sets.md) | [Comp 4.2: Pipeline Layouts (14.2.2.)](comp4_2_pipeline_layouts.md) |
