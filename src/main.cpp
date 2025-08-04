#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

bool checkShaderCompilationSuccess(unsigned shaderID, const char* shaderType, bool isShaderProgram) {
	int success = 1;
	char errorMessage[512];
	if (isShaderProgram) glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
	else glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (!success) {
		if (isShaderProgram) glGetProgramInfoLog(shaderID, 512, NULL, errorMessage);
		else glGetShaderInfoLog(shaderID, 512, NULL, errorMessage);
		std::cout << shaderType << " shader compilation failed. Error:" << errorMessage << std::endl;
		return false;
	}
	return true;
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

	const char* vertexShaderSource =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"out vec4 color;"
		"void main() {\n"
		"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
		"color = vec4(aColor, 1.0);\n"
		"}";
	unsigned vertexShader = 0;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	if (!checkShaderCompilationSuccess(vertexShader, "Vertex", false)) {
		glfwTerminate();
		return -1;
	}

	const char* fragmentShaderSource =
		"#version 330 core\n"
		"in vec4 color;"
		"out vec4 fragmentColor;\n"
		"void main() {\n"
		"fragmentColor = color;\n"
		"}";
	unsigned fragmentShader = 0;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	if (!checkShaderCompilationSuccess(fragmentShader, "Fragment", false)) {
		glfwTerminate();
		return -1;
	}

	unsigned shaderProgram = 0;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	if (!checkShaderCompilationSuccess(shaderProgram, "Shader program", true)) {
		glfwTerminate();
		return -1;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		mapInputToGlfwState(window);

		glClearColor(0.3f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(NULL);
		glUseProgram(NULL);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(shaderProgram);
	glfwTerminate();
	return 0;
}