#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void windowSizeAdjustCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mapInputToGlfwState(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(800, 800, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "GLFWwindow object creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD setup failed" << std::endl;
    }
    
    glViewport(0, 0, 800, 800);
    glfwSetFramebufferSizeCallback(window, windowSizeAdjustCallback);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        mapInputToGlfwState(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}