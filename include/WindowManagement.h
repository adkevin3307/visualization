#pragma once

#include "constant.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <vector>
#include <string>

#include "Camera.h"
#include "Shader.h"

using namespace std;

class WindowManagement {
private:
    int files_index;
    double last_xpos, last_ypos, rate;
    Camera camera;
    map<METHOD, Shader> shader_map;
    map<string, METHOD> methods;
    vector<vector<string>> files;
    GLFWwindow *window;

    static void error_callback(int error, const char *description);
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

    void set_callback();

    void generate_combo();
    void load(string filename, METHOD method);
    void gui();

public:
    WindowManagement();
    ~WindowManagement();

    void init();
    void main_loop();
};