# **Step 12.1: Data**

## **The Code**

The cube vertices coordinate:
```C++
    // Coordinate
	std::vector<glm::vec3> vert_coord = {
		// Top
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
```

The cube colors:
```C++
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
```

File: [step_12_1_data.cpp](../Code/step_12_1_data.cpp)

| Previous | Next |
|---|---|
| [Step 12: Vertex Buffers (12.1. and 12.7.)](vertex_buffers.md) | [Step 12.2: Coordinate and Color Buffers (12.1 and 12.7)](coordinate_and_color_buffers.md) |