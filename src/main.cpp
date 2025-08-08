#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
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
		}
		else if (!lineMode && !stopper) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			lineMode = true;
			stopper = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
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

	float vboData[] = {
		 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
	    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	unsigned vao = 0;
	glGenVertexArrays(1, &vao);
	unsigned vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vboData), vboData, GL_STATIC_DRAW);

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindVertexArray(NULL);

	Shader shaderProgram("src/shaders/vertex.txt", "src/shaders/fragment.txt");

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		mapInputToGlfwState(window);

		glClearColor(0.3f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.use();
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
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