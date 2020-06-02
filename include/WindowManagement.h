#pragma once

#include "constant.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <map>

#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Volume.h"

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

    Volume load_volume(string filename, vector<float> &histogram, vector<vector<float>> &distribution);
    void save_transfer_table(string filename, vector<vector<float>> &color, vector<vector<float>> &alpha);
    void load(Volume &volume, METHOD method, bool update);
    void gui();

public:
    WindowManagement();
    ~WindowManagement();

    void init();
    void main_loop();
};