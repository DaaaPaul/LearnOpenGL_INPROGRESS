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
        } else if (!lineMode && !stopper) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            lineMode = true;
            stopper = true;
        }
    } else if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
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

    float rectangleVerticies[] = {
        0.5f,  0.5f, 0.0f,
       -0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
    };
    unsigned indices[] = {
        0, 2, 1,
        1, 3, 2,
    };

    unsigned vao = 0;
    glGenVertexArrays(1, &vao);
    unsigned vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVerticies), rectangleVerticies, GL_STATIC_DRAW);
    unsigned ebo = 0;
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(NULL);
    glBindBuffer(GL_ARRAY_BUFFER, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);

    const char* vertexShaderSource = 
        "#version 330 core\n"
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

    const char* fragmentShaderSource = 
        "#version 330 core\n"
        "uniform vec4 color;"
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
        float time = glfwGetTime();
        float redValue = (sin(time) / 2.0f) + 0.5f;
        int colorUniformLocation = glGetUniformLocation(shaderProgram, "color");
        float newColor[] = { redValue, 0.5f, 0.5f, 1.0f };
        glUniform4fv(colorUniformLocation, 1, newColor);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(NULL);

        glfwSwapBuffers(window);
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}