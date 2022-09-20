# **Step 0: Window creation with GLFW**
## **Hints**
First of all, we aim for the documentation: [GLFW Documentation](https://www.glfw.org/documentation.html). After a few clicks, we find what we need here: [GLFW Vulkan Guide](https://www.glfw.org/docs/latest/vulkan_guide.html)

## **The Code**
Not all the includes are necessary for the context initialisation, but these are all the one needed throug this tutorial.

The while loop which will be used for rendering is already written, otherwise the window will close automatically.

```C++
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



	/********************/
	/* Game Loop        */
	/********************/
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	glfwTerminate();
	std::cout << "Programme terminated successfully" << std::endl;
}
```

File: [step_0_window_creation_with_glfw.cpp](../Code/step_0_window_creation_with_glfw.cpp)

| Previous | Next |
|---|---|
| [Environment configuration](environment_configuration.md) | [Step 1: Instances (4.2.)](instance.md) |