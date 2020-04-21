#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <map>

#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"

using namespace std;

class WindowManagement {
private:
    double last_xpos, last_ypos, rate;
    Camera camera;
    map<METHOD, Shader> shader_map;
    vector<Mesh> mesh;
    GLFWwindow *window;

    static void error_callback(int error, const char *description);
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

    void set_callback();

    void load(string filename, METHOD method);
    void set_gui(glm::vec4 &clip_plane);

public:
    WindowManagement();
    ~WindowManagement();

    void init();
    void main_loop();
};