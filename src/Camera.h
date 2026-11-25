#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;

    Camera(glm::vec3 position =
           glm::vec3(0.0f, 0.0f, 3.0f))
    {
        Position = position;

        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        Yaw = -90.0f;
        Pitch = 0.0f;

        MovementSpeed = 5.0f;
        MouseSensitivity = 0.1f;

        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(
            Position,
            Position + Front,
            Up
        );
    }

    void ProcessKeyboard(
        CameraMovement direction,
        float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        // Calculate movement direction on XZ plane
        glm::vec3 forward;

        forward.x = cos(glm::radians(Yaw));
        forward.y = 0.0f;
        forward.z = sin(glm::radians(Yaw));

        forward = glm::normalize(forward);

        glm::vec3 right =
            glm::normalize(
                glm::cross(forward, WorldUp)
            );

        if (direction == FORWARD)
            Position += forward * velocity;

        if (direction == BACKWARD)
            Position -= forward * velocity;

        if (direction == LEFT)
            Position -= right * velocity;

        if (direction == RIGHT)
            Position += right * velocity;
    }

    void ProcessMouseMovement(
        float xOffset,
        float yOffset)
    {
        xOffset *= MouseSensitivity;
        yOffset *= MouseSensitivity;

        Yaw += xOffset;
        Pitch += yOffset;

        if (Pitch > 89.0f)
            Pitch = 89.0f;

        if (Pitch < -89.0f)
            Pitch = -89.0f;

        updateCameraVectors();
    }

private:

    void updateCameraVectors()
    {
        glm::vec3 front;

        front.x =
            cos(glm::radians(Yaw)) *
            cos(glm::radians(Pitch));

        front.y =
            sin(glm::radians(Pitch));

        front.z =
            sin(glm::radians(Yaw)) *
            cos(glm::radians(Pitch));

        Front = glm::normalize(front);

        Right = glm::normalize(
            glm::cross(Front, WorldUp)
        );

        Up = glm::normalize(
            glm::cross(Right, Front)
        );
    }
};