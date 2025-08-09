#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "std_image.h"
#include <iostream>

const unsigned WINDOW_HEIGHT = 600;
const unsigned WINDOW_WIDTH = 800;
bool lineMode = false;
bool stopper = false;

void windowSizeAdjustCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mapInputToGlfwState(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
		if (lineMode && !stopper) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			lineMode = false;
			stopper = true;
		} else if (!lineMode && !stopper) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			lineMode = true;
			stopper = true;
		}
	} else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
		stopper = false;
	}
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "GLFWwindow object creation failed" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "GLAD setup failed" << std::endl;
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, windowSizeAdjustCallback);

	Shader shaderProgram("src/shaders/vertex.txt", "src/shaders/fragment.txt");

	const float vboData[] = {
		-0.5f, -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,    0.0f, 0.0f, 
	    -0.5f,  0.5f, 0.0f,    0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 
		 0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f, 
		 0.5f,  0.5f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 
	};
	const unsigned eboData[] = {
		0, 1, 2,
		3, 2, 1,
	};

	unsigned vao = 0;
	glGenVertexArrays(1, &vao);
	unsigned vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vboData), vboData, GL_STATIC_DRAW);
	unsigned ebo = 0;
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eboData), eboData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindVertexArray(NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

	unsigned texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int height = 0, width = 0, channels = 0;
	unsigned char* imageData = stbi_load("src/textures/container.jpg", &width, &height, &channels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, NULL);
	stbi_image_free(imageData);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		mapInputToGlfwState(window);

		glClearColor(0.3f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.use();
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, static_cast<void*>(0));
		glBindVertexArray(NULL);
		glUseProgram(NULL);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	shaderProgram.kill();
	glfwTerminate();
	return 0;
}