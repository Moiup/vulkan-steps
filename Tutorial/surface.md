# **Step 2: Surface**
## **Hints**
The function to call is [`glfwCreateWindowSurface`](https://www.glfw.org/docs/3.3/group__vulkan.html#ga1a24536bec3f80b08ead18e28e6ae965).

## **The Code**

Do not forget to destroy the surface at the end with: [`vkDestroySurfaceKHR`](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap33.html#vkDestroySurfaceKHR).

```C++
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
```

File: [step_2_surface.cpp](../Code/step_2_surface.cpp)

| Previous | Next |
|---|---|
| [Step 1: Instaces](instance.md) | [Step 3: Devices and Queues](devices_and_queues.md) |