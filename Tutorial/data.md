# **Step 12.1: Data**

## **The Code**

The cube vertices coordinate:
```C++
    // Coordinate
	std::vector<glm::vec3> vert_coord = {
		// Avant
		glm::vec3(1,  1, -1),
		glm::vec3(-1, -1, -1),
		glm::vec3(-1,  1, -1),
		glm::vec3(-1, -1, -1),
		glm::vec3(1,  1, -1),
		glm::vec3(1, -1, -1),
		// Droite
		glm::vec3(1, -1, -1),
		glm::vec3(1,  1,  1),
		glm::vec3(1,  1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(1, -1,  1),
		glm::vec3(1,  1,  1),
		// Arriere
		glm::vec3(-1, -1,  1),
		glm::vec3(1, -1,  1),
		glm::vec3(1,  1,  1),
		glm::vec3(-1, -1,  1),
		glm::vec3(1,  1,  1),
		glm::vec3(-1,  1,  1),
		// Gauche
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, -1,  1),
		glm::vec3(-1,  1,  1),
		glm::vec3(-1, -1, -1),
		glm::vec3(-1,  1,  1),
		glm::vec3(-1,  1, -1),
		// Bas
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, -1,  1),
		glm::vec3(1, -1, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(-1, -1,  1),
		glm::vec3(1, -1,  1),
		// Haut
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
		// Devant
		glm::vec4(1, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		glm::vec4(1, 0, 0, 1),
		// Droite
		glm::vec4(0, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		glm::vec4(0, 0, 1, 1),
		// Arrière
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0, 1, 0, 1),
		glm::vec4(0, 1, 0, 1),
		// Gauche
		glm::vec4(0, 1, 1, 1),
		glm::vec4(0, 1, 1, 1),
		glm::vec4(0, 1, 1, 1),
		glm::vec4(0, 1, 1, 1),
		glm::vec4(0, 1, 1, 1),
		glm::vec4(0, 1, 1, 1),
		// Haut
		glm::vec4(1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		glm::vec4(1, 0, 1, 1),
		// Bas
		glm::vec4(1, 1, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(1, 1, 0, 1),
		glm::vec4(1, 1, 0, 1)
	};
```

| Previous | Next |
|---|---|
| [Step 12: Vertex Buffers (12.1. and 12.7.)](vertex_buffers.md) | [Step 12.2: Coordinate and Color Buffers (12.1 and 12.7)](coordinate_and_color_buffers.md) |