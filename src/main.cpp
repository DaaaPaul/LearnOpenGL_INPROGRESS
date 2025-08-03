#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const unsigned WINDOW_HEIGHT = 600;
const unsigned WINDOW_WIDTH = 800;

void windowSizeAdjustCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mapInputToGlfwState(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

bool checkShaderCompilationSuccess(unsigned shaderID, const char* shaderType, bool isShaderProgram) {
    int success = 1;
    char errorMessage[512];
    if (isShaderProgram) glGetProgramiv(shaderID, GL_COMPILE_STATUS, &success);
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

    float triangleVerticies[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };

    unsigned vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVerticies), triangleVerticies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
        "}";
    unsigned vertexShader = 0;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    if (!checkShaderCompilationSuccess(vertexShader, "Vertex", false)) {
        glfwTerminate();
        return -1;
    }

    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 fragmentColor;\n"
        "void main() {\n"
        "fragmentColor = vec4(1.0f, 1.0f, 0.2f, 1.0f);\n"
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

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}