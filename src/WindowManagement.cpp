#include "WindowManagement.h"

#include "constant.h"

#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <dirent.h>

#include "Method.h"
#include "IsoSurface.h"
#include "Transformation.h"

using namespace std;

WindowManagement::WindowManagement()
    : last_xpos(0.0), last_ypos(0.0), rate(7.0)
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
    ImGui::SetWindowSize("Controller", ImVec2(width / 3.0, height / 3.0));
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
        default:
            break;
    }
}

void WindowManagement::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (ImGui::IsAnyMouseDown() && ImGui::IsAnyWindowFocused()) return;

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
    if (ImGui::IsAnyWindowHovered()) {
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
        return;
    }

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

vector<string> WindowManagement::all_files()
{
    DIR *dp;
    dirent *dirp;
    vector<string> result;

    if ((dp = opendir("./Data/Scalar/")) != NULL) {
        while ((dirp = readdir(dp)) != NULL) {
            string temp = dirp->d_name;
            size_t index = temp.find(".inf");

            if (index != string::npos) result.push_back(temp.substr(0, index));
        }
    }

    sort(result.begin(), result.end());

    return result;
}

void WindowManagement::load(string filename, METHOD method)
{
    Method *render_method;

    cout << "==================== Info ====================" << '\n';

    try {
        switch (method) {
        case (METHOD::ISOSURFACE):
            render_method = new IsoSurface("./Data/Scalar/" + filename + ".inf", "./Data/Scalar/" + filename + ".raw");
            render_method->run();
            break;
        default:
            break;
        }

        this->mesh.push_back(make_pair(Mesh(render_method), method));
        for (size_t i = 0; i < this->mesh.size(); i++) {
            this->mesh[i].first.init();
        }

        delete render_method;
    }
    catch (const runtime_error &error) {
        cerr << error.what() << '\n';
    }

    cout << "==============================================" << '\n';
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init("#version 440");

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->set_callback();

    this->shader_map[METHOD::ISOSURFACE] = Shader("./src/shader/vertex.glsl", "./src/shader/fragment.glsl");

    for (auto it = this->shader_map.begin(); it != this->shader_map.end(); it++) {
        it->second.use();
    }
}

void WindowManagement::main_loop()
{
    bool open = true;

    string method = "Iso Surface";
    map<string, METHOD> methods;
    methods["Iso Surface"] = METHOD::ISOSURFACE;

    string filename = "engine";
    vector<string> filenames = this->all_files();

    float clip_normal[] = {0.0, 0.0, 0.0}, clip_distance = 1.0;

    while (!glfwWindowShouldClose(this->window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(WIDTH / 3, HEIGHT / 3), ImGuiCond_Once);

        // render GUI
        ImGui::Begin("Controller", &open, ImGuiWindowFlags_NoMove);
        ImGui::SetWindowFontScale(1.2);

        if (ImGui::BeginCombo("## Method", method.c_str())) {
            for (auto it = methods.begin(); it != methods.end(); it++) {
                bool selected = (method == it->first);

                if (ImGui::Selectable(it->first.c_str(), selected)) method = it->first;
                if (selected) ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("## Filename", filename.c_str())) {
            for (size_t i = 0; i < filenames.size(); i++) {
                bool selected = (filename == filenames[i]);

                if (ImGui::Selectable(filenames[i].c_str(), selected)) filename = filenames[i];
                if (selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine();
        if (ImGui::Button("Clean")) this->mesh.clear();

        ImGui::SameLine();
        if (ImGui::Button("Load")) this->load(filename, methods[method]);

        ImGui::SetWindowFontScale(1.0);
        ImGui::SliderFloat3("Clip Plane Normal", clip_normal, -1.0, 1.0);
        ImGui::SliderFloat("Clip Plane Distanse", &clip_distance, -150.0, 150.0);

        ImGui::End();

        for (size_t i = 0; i < this->mesh.size(); i++) {
            glm::vec3 clip_plane = glm::make_vec3(clip_normal);
            if (glm::length(clip_plane) > EPSILON) clip_plane = glm::normalize(clip_plane);

            this->shader_map[mesh[i].second].set_uniform("clip_plane", glm::vec4(clip_plane, clip_distance));

            this->shader_map[mesh[i].second].set_uniform("view_pos", this->camera.position());
            this->shader_map[mesh[i].second].set_uniform("light_pos", this->camera.position());
            this->shader_map[mesh[i].second].set_uniform("light_color", glm::vec3(1.0, 1.0, 1.0));

            Transformation transformation(this->shader_map[mesh[i].second]);
            transformation.set_projection(WIDTH, HEIGHT, this->rate, 0.0, 500.0);
            transformation.set_view(this->camera.view_matrix());

            this->mesh[i].first.transform(transformation);
            transformation.set();

            this->shader_map[this->mesh[i].second].set_uniform("object_color", glm::vec4(0.41, 0.37, 0.89, 1.0));
            this->mesh[i].first.draw(GL_FILL);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(this->window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(this->window);
    glfwTerminate();
}