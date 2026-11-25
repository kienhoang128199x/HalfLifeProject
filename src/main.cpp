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

float playerSpeed = 5.0f;

float gravity = -9.81f;

float verticalVelocity = 0.0f;

float jumpForce = 5.0f;

float groundHeight = 0.0f;

bool isGrounded = true;

const unsigned int WIDTH = 1280;
const unsigned int HEIGHT = 720;

// Camera
Camera camera;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;

bool firstMouse = true;


// ============================================
// LOAD SHADER
// ============================================

std::string loadShader(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Cannot open shader file: " + path
        );
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void processJump(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS
        && isGrounded)
    {
        verticalVelocity = jumpForce;
        isGrounded = false;
    }
}

void updatePhysics(float deltaTime)
{
    // Apply gravity
    verticalVelocity += gravity * deltaTime;

    // Update player position
    camera.Position.y +=
        verticalVelocity * deltaTime;

    // Ground collision
    if (camera.Position.y <= groundHeight)
    {
        camera.Position.y = groundHeight;

        verticalVelocity = 0.0f;

        isGrounded = true;
    }
}

// ============================================
// COMPILE SHADER
// ============================================

GLuint compileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);

    const char* sourceCode = source.c_str();

    glShaderSource(shader, 1, &sourceCode, nullptr);
    glCompileShader(shader);

    GLint success;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success)
    {
        char infoLog[512];

        glGetShaderInfoLog(
            shader,
            512,
            nullptr,
            infoLog
        );

        std::cerr << "Shader compilation error:\n"
                  << infoLog << std::endl;

        glDeleteShader(shader);

        throw std::runtime_error(
            "Shader compilation failed"
        );
    }

    return shader;
}


// ============================================
// CREATE SHADER PROGRAM
// ============================================

GLuint createShaderProgram()
{
    std::string vertexSource =
        loadShader(
            std::string(ASSET_DIR) +
            "/shaders/vertex.glsl"
        );

    std::string fragmentSource =
        loadShader(
            std::string(ASSET_DIR) +
            "/shaders/fragment.glsl"
        );

    GLuint vertexShader =
        compileShader(GL_VERTEX_SHADER, vertexSource);

    GLuint fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    GLint success;

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );

    if (!success)
    {
        char infoLog[512];

        glGetProgramInfoLog(
            program,
            512,
            nullptr,
            infoLog
        );

        std::cerr << "Shader linking error:\n"
                  << infoLog << std::endl;

        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        throw std::runtime_error(
            "Shader linking failed"
        );
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}


// ============================================
// MOUSE CALLBACK
// ============================================

void mouse_callback(
    GLFWwindow* window,
    double xpos,
    double ypos)
{
    if (firstMouse)
    {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);

        firstMouse = false;
    }

    float xOffset =
        static_cast<float>(xpos) - lastX;

    float yOffset =
        lastY - static_cast<float>(ypos);

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(
        xOffset,
        yOffset
    );
}


// ============================================
// KEYBOARD INPUT
// ============================================

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)
        == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W)
        == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S)
        == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A)
        == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D)
        == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    processJump(window);
}

// ============================================
// DRAW CUBE
// ============================================

void drawCube(
    GLuint shaderProgram,
    GLuint VAO,
    const glm::mat4& model,
    const glm::vec3& color,
    bool useVertexColor)
{
    GLint modelLoc =
        glGetUniformLocation(
            shaderProgram,
            "model"
        );

    GLint colorLoc =
        glGetUniformLocation(
            shaderProgram,
            "objectColor"
        );

    GLint useVertexColorLoc =
        glGetUniformLocation(
            shaderProgram,
            "useVertexColor"
        );

    glUniformMatrix4fv(
        modelLoc,
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );

    glUniform3fv(
        colorLoc,
        1,
        glm::value_ptr(color)
    );

    glUniform1i(
        useVertexColorLoc,
        useVertexColor ? 1 : 0
    );

    glBindVertexArray(VAO);

    glDrawArrays(
        GL_TRIANGLES,
        0,
        36
    );
}


// ============================================
// MAIN
// ============================================

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW"
                  << std::endl;

        return -1;
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    // Create window
    GLFWwindow* window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "HalfLife Clone - Milestone 4",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Failed to create GLFW window"
                  << std::endl;

        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW"
                  << std::endl;

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
    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );

    glfwSetCursorPosCallback(
        window,
        mouse_callback
    );

    GLuint shaderProgram = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;

    try
    {
        // Create shader program
        shaderProgram = createShaderProgram();


        // ====================================
        // CUBE VERTICES
        // ====================================

        // Each vertex:
        // Position: x, y, z
        // Color:    r, g, b

        float vertices[] = {

            // Front face - Red
            -0.5f, -0.5f,  0.5f,  1, 0, 0,
             0.5f, -0.5f,  0.5f,  1, 0, 0,
             0.5f,  0.5f,  0.5f,  1, 0, 0,

             0.5f,  0.5f,  0.5f,  1, 0, 0,
            -0.5f,  0.5f,  0.5f,  1, 0, 0,
            -0.5f, -0.5f,  0.5f,  1, 0, 0,


            // Back face - Green
            -0.5f, -0.5f, -0.5f,  0, 1, 0,
             0.5f, -0.5f, -0.5f,  0, 1, 0,
             0.5f,  0.5f, -0.5f,  0, 1, 0,

             0.5f,  0.5f, -0.5f,  0, 1, 0,
            -0.5f,  0.5f, -0.5f,  0, 1, 0,
            -0.5f, -0.5f, -0.5f,  0, 1, 0,


            // Left face - Blue
            -0.5f,  0.5f,  0.5f,  0, 0, 1,
            -0.5f,  0.5f, -0.5f,  0, 0, 1,
            -0.5f, -0.5f, -0.5f,  0, 0, 1,

            -0.5f, -0.5f, -0.5f,  0, 0, 1,
            -0.5f, -0.5f,  0.5f,  0, 0, 1,
            -0.5f,  0.5f,  0.5f,  0, 0, 1,


            // Right face - Yellow
             0.5f,  0.5f,  0.5f,  1, 1, 0,
             0.5f,  0.5f, -0.5f,  1, 1, 0,
             0.5f, -0.5f, -0.5f,  1, 1, 0,

             0.5f, -0.5f, -0.5f,  1, 1, 0,
             0.5f, -0.5f,  0.5f,  1, 1, 0,
             0.5f,  0.5f,  0.5f,  1, 1, 0,


            // Top face - Cyan
            -0.5f,  0.5f, -0.5f,  0, 1, 1,
             0.5f,  0.5f, -0.5f,  0, 1, 1,
             0.5f,  0.5f,  0.5f,  0, 1, 1,

             0.5f,  0.5f,  0.5f,  0, 1, 1,
            -0.5f,  0.5f,  0.5f,  0, 1, 1,
            -0.5f,  0.5f, -0.5f,  0, 1, 1,


            // Bottom face - Magenta
            -0.5f, -0.5f, -0.5f,  1, 0, 1,
             0.5f, -0.5f, -0.5f,  1, 0, 1,
             0.5f, -0.5f,  0.5f,  1, 0, 1,

             0.5f, -0.5f,  0.5f,  1, 0, 1,
            -0.5f, -0.5f,  0.5f,  1, 0, 1,
            -0.5f, -0.5f, -0.5f,  1, 0, 1
        };


        // ====================================
        // CREATE VAO / VBO
        // ====================================

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(
            GL_ARRAY_BUFFER,
            VBO
        );

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


        // ====================================
        // GAME LOOP
        // ====================================

        while (!glfwWindowShouldClose(window))
        {
            // Calculate delta time
            float currentFrame =
                static_cast<float>(glfwGetTime());

            deltaTime = currentFrame - lastFrame;

            lastFrame = currentFrame;

            // Process input
            processInput(window);

            updatePhysics(deltaTime);    
            
            // Clear screen
            glClearColor(
                0.05f,
                0.05f,
                0.08f,
                1.0f
            );

            glClear(
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT
            );


            // Activate shader
            glUseProgram(shaderProgram);


            // View matrix
            glm::mat4 view =
                camera.GetViewMatrix();

            // Projection matrix
            glm::mat4 projection =
                glm::perspective(
                    glm::radians(45.0f),
                    static_cast<float>(WIDTH) / HEIGHT,
                    0.1f,
                    100.0f
                );


            // Send View Matrix
            GLint viewLoc =
                glGetUniformLocation(
                    shaderProgram,
                    "view"
                );

            glUniformMatrix4fv(
                viewLoc,
                1,
                GL_FALSE,
                glm::value_ptr(view)
            );


            // Send Projection Matrix
            GLint projectionLoc =
                glGetUniformLocation(
                    shaderProgram,
                    "projection"
                );

            glUniformMatrix4fv(
                projectionLoc,
                1,
                GL_FALSE,
                glm::value_ptr(projection)
            );


            // ====================================
            // FLOOR
            // ====================================

            glm::mat4 floorModel(1.0f);

            floorModel = glm::translate(
                floorModel,
                glm::vec3(0.0f, -1.0f, 0.0f)
            );

            floorModel = glm::scale(
                floorModel,
                glm::vec3(20.0f, 0.2f, 20.0f)
            );

            drawCube(
                shaderProgram,
                VAO,
                floorModel,
                glm::vec3(0.4f, 0.4f, 0.4f),
                false
            );


            // ====================================
            // BACK WALL
            // ====================================

            glm::mat4 backWall(1.0f);

            backWall = glm::translate(
                backWall,
                glm::vec3(0.0f, 1.5f, -10.0f)
            );

            backWall = glm::scale(
                backWall,
                glm::vec3(20.0f, 5.0f, 0.2f)
            );

            drawCube(
                shaderProgram,
                VAO,
                backWall,
                glm::vec3(0.7f, 0.3f, 0.3f),
                false
            );


            // ====================================
            // FRONT WALL
            // ====================================

            glm::mat4 frontWall(1.0f);

            frontWall = glm::translate(
                frontWall,
                glm::vec3(0.0f, 1.5f, 10.0f)
            );

            frontWall = glm::scale(
                frontWall,
                glm::vec3(20.0f, 5.0f, 0.2f)
            );

            drawCube(
                shaderProgram,
                VAO,
                frontWall,
                glm::vec3(0.3f, 0.3f, 0.7f),
                false
            );


            // ====================================
            // LEFT WALL
            // ====================================

            glm::mat4 leftWall(1.0f);

            leftWall = glm::translate(
                leftWall,
                glm::vec3(-10.0f, 1.5f, 0.0f)
            );

            leftWall = glm::scale(
                leftWall,
                glm::vec3(0.2f, 5.0f, 20.0f)
            );

            drawCube(
                shaderProgram,
                VAO,
                leftWall,
                glm::vec3(0.3f, 0.7f, 0.3f),
                false
            );


            // ====================================
            // RIGHT WALL
            // ====================================

            glm::mat4 rightWall(1.0f);

            rightWall = glm::translate(
                rightWall,
                glm::vec3(10.0f, 1.5f, 0.0f)
            );

            rightWall = glm::scale(
                rightWall,
                glm::vec3(0.2f, 5.0f, 20.0f)
            );

            drawCube(
                shaderProgram,
                VAO,
                rightWall,
                glm::vec3(0.7f, 0.7f, 0.3f),
                false
            );


            // ====================================
            // ROTATING CUBE
            // ====================================

            glm::mat4 cubeModel(1.0f);

            cubeModel = glm::rotate(
                cubeModel,
                static_cast<float>(glfwGetTime()),
                glm::vec3(0.5f, 1.0f, 0.0f)
            );

            drawCube(
                shaderProgram,
                VAO,
                cubeModel,
                glm::vec3(1.0f, 1.0f, 1.0f),
                true
            );


            // ====================================
            // SWAP BUFFERS
            // ====================================

            glfwSwapBuffers(window);

            glfwPollEvents();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: "
                  << e.what()
                  << std::endl;
    }


    // ====================================
    // CLEANUP
    // ====================================

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