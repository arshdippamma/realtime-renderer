#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

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
        -0.7f, 0.05f, 0.3f, 0.2f, 0.7f, 0.9f, 1.0f,
        -0.1f, 0.5f, -0.4f, 0.1f, 0.6f, 0.1f, 1.0f,
        0.5f, 0.2f, 0.0f, 1.0f, 0.3f, 0.0f, 1.0f,
    };

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Time
    GLint uTimeLocation = glGetUniformLocation(shaderProgram, "uTime");

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.15f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float time = (float)glfwGetTime();
        glUniform1f(uTimeLocation, time);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
