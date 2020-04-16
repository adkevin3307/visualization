#include "Camera.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    this->_radius = 300.0f;
    this->_fov = 45.0;
    this->_yaw = 90.0;
    this->_pitch = 0.0;
    this->_sensitivity = 0.1;

    this->_position = glm::vec3(0.0, 0.0, 0.0);
    this->_center = glm::vec3(0.0, 0.0, 0.0);
    this->_up = glm::vec3(0.0, 0.0, 0.0);
    this->_right = glm::vec3(0.0, 0.0, 0.0);

    this->update();
}

Camera::~Camera()
{

}

void Camera::update()
{
    glm::vec3 temp;
    temp.x = cos(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));
    temp.y = sin(glm::radians(this->_pitch));
    temp.z = sin(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));

    glm::vec3 front = glm::normalize(temp);

    this->_position = this->_center - this->_radius * front;
    this->_right = glm::normalize(glm::cross(front, glm::vec3(0.0, 1.0, 0.0)));
    this->_up = glm::normalize(glm::cross(this->_right, front));
}

void Camera::process_mouse(BUTTON button, double xoffset, double yoffset)
{
    if (button == BUTTON::LEFT) {
        this->_yaw += xoffset * this->_sensitivity;
        this->_pitch += yoffset * this->_sensitivity;

        if (this->_pitch > 89.0f) this->_pitch = 89.0f;
        if (this->_pitch < -89.0f) this->_pitch = -89.0f;
    }
    else if (button == BUTTON::RIGHT) {
        this->_center += (float)(xoffset * this->_sensitivity) * this->_right;
        this->_center += (float)(yoffset * this->_sensitivity) * this->_up;
    }

    this->update();
}

void Camera::reset()
{
    this->_yaw = 90.0;
    this->_pitch = 0.0;

    this->_center = glm::vec3(0.0, 0.0, 0.0);

    this->update();
}

glm::mat4 Camera::view_matrix()
{
    return glm::lookAt(this->_position, this->_center, this->_up);
}

glm::vec3 Camera::center()
{
    return this->_center;
}

glm::vec3 Camera::up()
{
    return this->_up;
}

glm::vec3 Camera::position()
{
    return this->_position;
}