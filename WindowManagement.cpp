#include "WindowManagement.h"

#include "include/constant.h"

#include <glm/ext.hpp>
#include <iostream>
#include <stdexcept>

#include "Transformation.h"

using namespace std;

WindowManagement::WindowManagement()
    : last_xpos(0.0), last_ypos(0.0), rate(7.0), clip(0.0)
{

}

WindowManagement::~WindowManagement()
{

}

void WindowManagement::error_callback(int error, const char *description)
{
    cerr << "Error: " << description << '\n';
}

void WindowManagement::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void WindowManagement::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_SPACE:
            this->camera.reset();
            break;
        case GLFW_KEY_KP_ADD:
            this->clip += 1.0;
            break;
        case GLFW_KEY_KP_SUBTRACT:
            this->clip -= 1.0;
            break;
        default:
            break;
    }
}

void WindowManagement::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        this->camera.process_mouse(BUTTON::LEFT, xpos - this->last_xpos, this->last_ypos - ypos);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        this->camera.process_mouse(BUTTON::RIGHT, this->last_xpos - xpos, ypos - this->last_ypos);
    }

    this->last_xpos = xpos;
    this->last_ypos = ypos;
}

void WindowManagement::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    this->rate += yoffset;
    if (this->rate < 0.1) this->rate = 0.1;
}

void WindowManagement::set_callback()
{
    glfwSetWindowUserPointer(this->window, this);

    auto framebufferSizeCallback = [](GLFWwindow *window, int width, int height) {
        static_cast<WindowManagement*>(glfwGetWindowUserPointer(window))->framebuffer_size_callback(window, width, height);
    };

    auto keyCallback = [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        static_cast<WindowManagement*>(glfwGetWindowUserPointer(window))->key_callback(window, key, scancode, action, mods);
    };

    auto mouseCallback = [](GLFWwindow *window, double xpos, double ypos) {
        static_cast<WindowManagement*>(glfwGetWindowUserPointer(window))->mouse_callback(window, xpos, ypos);
    };

    auto scrollCallback = [](GLFWwindow *window, double xoffset, double yoffset) {
        static_cast<WindowManagement*>(glfwGetWindowUserPointer(window))->scroll_callback(window, xoffset, yoffset);
    };

    glfwSetFramebufferSizeCallback(this->window, framebufferSizeCallback);
    glfwSetKeyCallback(this->window, keyCallback);
    glfwSetCursorPosCallback(this->window, mouseCallback);
    glfwSetScrollCallback(this->window, scrollCallback);
}

void WindowManagement::init()
{
    glfwSetErrorCallback(WindowManagement::error_callback);

    if (!glfwInit()) throw runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

    this->window = glfwCreateWindow(WIDTH, HEIGHT, "window", NULL, NULL);

    if (!this->window) {
        glfwTerminate();
        throw runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(this->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw runtime_error("Failed to initialize GLAD");

    glEnable(GL_DEPTH_TEST);

    this->set_callback();
}

void WindowManagement::main_loop(Mesh &mesh, Shader &shader)
{
    while (!glfwWindowShouldClose(this->window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Transformation transformation(shader);
        transformation.set_projection(WIDTH, HEIGHT, this->rate, 0.0, 500.0);
        transformation.set_view(this->camera.view_matrix());
        mesh.transform(transformation);
        transformation.set();

        shader.set_uniform("clip_plane", glm::vec4(0.0, 0.0, 1.0, this->clip));

        shader.set_uniform("view_pos", this->camera.position());
        shader.set_uniform("light_pos", this->camera.position());
        shader.set_uniform("light_color", glm::vec3(1.0, 1.0, 1.0));

        shader.set_uniform("object_color", glm::vec3(0.41, 0.37, 0.89));
        mesh.draw(GL_FILL);

        shader.set_uniform("object_color", glm::vec3(0.36, 0.32, 0.84));
        mesh.draw(GL_LINE);

        glfwSwapBuffers(this->window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}