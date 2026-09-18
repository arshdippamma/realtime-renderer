#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string readFile(const char* path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::fprintf(stderr, "Failed to open file: %s\n", path);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

int main() {
    if (!glfwInit()) {
        std::fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // OpenGL 4.1 core context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // for mac

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Renderer", nullptr, nullptr);
    if (!window) {
        std::fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Loading OpenGL fn ptrs
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    std::printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    // Loading shader sources from disk
    std::string vertexShaderSource = readFile("shaders/triangle.vert");
    std::string fragmentShaderSource = readFile("shaders/triangle.frag");
    const char* vertexShaderSourceCStr = vertexShaderSource.c_str();
    const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str();

    // Creating vertex shader object
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSourceCStr, nullptr);
    glCompileShader(vertexShader);

    // Checking if it succeeded
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::fprintf(stderr, "Failed to compile vertex shader:\n%s\n", infoLog);
    }

    // Creating vertex shader object and verifying
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::fprintf(stderr, "Failed to compile fragment shader:\n%s\n", infoLog);
    }

    // Linking compiled shaders into one shader program
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    // Checking if it succeeded
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::fprintf(stderr, "Failed to link shader program:\n%s\n", infoLog);
    }

    // Cleaning up shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.7f, 0.2f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.1f, 1.0f, 0.9f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.7f, 0.8f, 0.2f, 1.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.1f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.6f, 0.6f, 0.8f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.4f, 0.7f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.2f, 0.0f, 1.0f, 1.0f
    };

    unsigned int indices[] = {
        // Front
        1, 3, 5,
        3, 5, 7,

        // Back
        0, 2, 4,
        2, 4, 6,

        // Top
        2, 3, 7,
        2, 6, 7,

        // Bottom
        0, 1, 4,
        1, 4, 5,

        // Left
        0, 1, 2,
        1, 2, 3,

        // Right
        4, 5, 7,
        4, 6, 7
    };

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Time
    GLint uTimeLocation = glGetUniformLocation(shaderProgram, "uTime");

    GLint uModelLocation = glGetUniformLocation(shaderProgram, "uModel");
    GLint uViewLocation = glGetUniformLocation(shaderProgram, "uView");
    GLint uProjectionLocation = glGetUniformLocation(shaderProgram, "uProjection");

    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.15f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float time = (float)glfwGetTime();
        glUniform1f(uTimeLocation, time);

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), time, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

        glUniformMatrix4fv(uModelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uViewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
