#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

// Camera
Camera camera;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;

bool firstMouse = true;


// Read shader file
std::string loadShader(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open shader file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}


// Compile shader
GLuint compileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);

    const char* sourceCode = source.c_str();

    glShaderSource(shader, 1, &sourceCode, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];

        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        std::cerr << "Shader compilation error:\n"
                  << infoLog << std::endl;

        glDeleteShader(shader);

        throw std::runtime_error("Shader compilation failed");
    }

    return shader;
}


// Create shader program
GLuint createShaderProgram()
{
    std::string vertexSource =
        loadShader(std::string(ASSET_DIR) + "/shaders/vertex.glsl");

    std::string fragmentSource =
        loadShader(std::string(ASSET_DIR) + "/shaders/fragment.glsl");

    GLuint vertexShader =
        compileShader(GL_VERTEX_SHADER, vertexSource);

    GLuint fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];

        glGetProgramInfoLog(program, 512, nullptr, infoLog);

        std::cerr << "Shader linking error:\n"
                  << infoLog << std::endl;

        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::runtime_error("Shader linking failed");
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}


// Mouse callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);

        firstMouse = false;
    }

    float xOffset = static_cast<float>(xpos) - lastX;
    float yOffset = lastY - static_cast<float>(ypos);

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(xOffset, yOffset);
}


// Keyboard input
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}


int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "HalfLife Clone",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;

        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    std::cout << "OpenGL Version: "
              << glGetString(GL_VERSION)
              << std::endl;

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, mouse_callback);

    // Create shader program
    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;

    try
    {
        shaderProgram = createShaderProgram();

        // Cube vertices
        // Each vertex: position (x,y,z) + color (r,g,b)

        float vertices[] = {

            // Front face - Red
            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,


            // Back face - Green
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

             0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,


            // Left face - Blue
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,


            // Right face - Yellow
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,

             0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,


            // Top face - Cyan
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,

             0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,


            // Bottom face - Magenta
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f
        };


        // Create VAO and VBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        // Position attribute
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(float),
            (void*)0
        );

        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            6 * sizeof(float),
            (void*)(3 * sizeof(float))
        );

        glEnableVertexAttribArray(1);

        glBindVertexArray(0);


        // Game loop
        while (!glfwWindowShouldClose(window))
        {
            // Calculate delta time
            float currentFrame =
                static_cast<float>(glfwGetTime());

            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            // Process input
            processInput(window);

            // Clear screen
            glClearColor(0.05f, 0.05f, 0.08f, 1.0f);

            glClear(
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT
            );

            // Activate shader
            glUseProgram(shaderProgram);

            // Model matrix
            glm::mat4 model(1.0f);

            model = glm::rotate(
                model,
                static_cast<float>(glfwGetTime()),
                glm::vec3(0.5f, 1.0f, 0.0f)
            );

            // View matrix
            glm::mat4 view = camera.GetViewMatrix();

            // Projection matrix
            glm::mat4 projection = glm::perspective(
                glm::radians(45.0f),
                static_cast<float>(WIDTH) / HEIGHT,
                0.1f,
                100.0f
            );

            // Send matrices to shader
            GLint modelLoc =
                glGetUniformLocation(shaderProgram, "model");

            GLint viewLoc =
                glGetUniformLocation(shaderProgram, "view");

            GLint projectionLoc =
                glGetUniformLocation(shaderProgram, "projection");

            glUniformMatrix4fv(
                modelLoc,
                1,
                GL_FALSE,
                glm::value_ptr(model)
            );

            glUniformMatrix4fv(
                viewLoc,
                1,
                GL_FALSE,
                glm::value_ptr(view)
            );

            glUniformMatrix4fv(
                projectionLoc,
                1,
                GL_FALSE,
                glm::value_ptr(projection)
            );

            // Render cube
            glBindVertexArray(VAO);

            glDrawArrays(GL_TRIANGLES, 0, 36);

            // Swap buffers and poll events
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Cleanup
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);

    if (VBO != 0)
        glDeleteBuffers(1, &VBO);

    if (shaderProgram != 0)
        glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}