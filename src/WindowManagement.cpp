#include "WindowManagement.h"

#include <glm/glm.hpp>
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
#include <cmath>

#include "Method.h"
#include "IsoSurface.h"
#include "Slicing.h"
#include "Transformation.h"

using namespace std;

WindowManagement::WindowManagement()
    : last_xpos(0.0), last_ypos(0.0), rate(7.0)
{
}

WindowManagement::~WindowManagement()
{
}

void APIENTRY debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    // ignore non-significant error / warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    cout << "Debug message (" << id << "): " << message << '\n';

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            cout << "Source: API" << '\n';
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            cout << "Source: Window System" << '\n';
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            cout << "Source: Shader Compiler" << '\n';
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            cout << "Source: Third Party" << '\n';
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            cout << "Source: Application" << '\n';
            break;
        case GL_DEBUG_SOURCE_OTHER:
            cout << "Source: Other" << '\n';
            break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            cout << "Type: Error" << '\n';
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            cout << "Type: Deprecated Behaviour" << '\n';
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            cout << "Type: Undefined Behaviour" << '\n';
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            cout << "Type: Portability" << '\n';
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            cout << "Type: Performance" << '\n';
            break;
        case GL_DEBUG_TYPE_MARKER:
            cout << "Type: Marker" << '\n';
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            cout << "Type: Push Group" << '\n';
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            cout << "Type: Pop Group" << '\b';
            break;
        case GL_DEBUG_TYPE_OTHER:
            cout << "Type: Other" << '\n';
            break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            cout << "Severity: high" << '\n';
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            cout << "Severity: medium" << '\n';
            break;
        case GL_DEBUG_SEVERITY_LOW:
            cout << "Severity: low" << '\n';
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            cout << "Severity: notification" << '\n';
            break;
    }
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
        static_cast<WindowManagement *>(glfwGetWindowUserPointer(window))->framebuffer_size_callback(window, width, height);
    };

    auto keyCallback = [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        static_cast<WindowManagement *>(glfwGetWindowUserPointer(window))->key_callback(window, key, scancode, action, mods);
    };

    auto mouseCallback = [](GLFWwindow *window, double xpos, double ypos) {
        static_cast<WindowManagement *>(glfwGetWindowUserPointer(window))->mouse_callback(window, xpos, ypos);
    };

    auto scrollCallback = [](GLFWwindow *window, double xoffset, double yoffset) {
        static_cast<WindowManagement *>(glfwGetWindowUserPointer(window))->scroll_callback(window, xoffset, yoffset);
    };

    glfwSetFramebufferSizeCallback(this->window, framebufferSizeCallback);
    glfwSetKeyCallback(this->window, keyCallback);
    glfwSetCursorPosCallback(this->window, mouseCallback);
    glfwSetScrollCallback(this->window, scrollCallback);
}

vector<float> WindowManagement::load(string filename, METHOD method, bool first)
{
    vector<int> histogram_int;
    string inf_file = "./Data/Scalar/" + filename + ".inf", raw_file = "./Data/Scalar/" + filename + ".raw";

    this->shader_map[method].use();

    if (first) cout << "==================== Info ====================" << '\n';

    try {
        switch (method) {
            case (METHOD::ISOSURFACE): {
                IsoSurface iso_surface(inf_file, raw_file);
                iso_surface.run();

                Mesh temp_mesh(iso_surface, METHOD::ISOSURFACE);
                temp_mesh.init();

                this->mesh.push_back(temp_mesh);

                histogram_int = ((Method*)(&iso_surface))->histogram();
                break;
            }
            case (METHOD::SLICING): {
                static Slicing slicing;

                if (first) {
                    slicing = Slicing(inf_file, raw_file);
                    slicing.run(this->camera.position());

                    Mesh temp_mesh(slicing, METHOD::SLICING);
                    temp_mesh.enable_texture(2);
                    temp_mesh.init();
                    temp_mesh.init_texture(GL_TEXTURE_1D, 0);
                    temp_mesh.init_texture(GL_TEXTURE_3D, 1);
                    temp_mesh.set_texture(0, slicing.texture_1d(), slicing.texture_1d_shape());
                    temp_mesh.set_texture(1, slicing.texture_3d(), slicing.texture_3d_shape());

                    this->mesh.push_back(temp_mesh);

                    histogram_int = ((Method*)(&slicing))->histogram();
                }

                if (slicing.run(this->camera.position())) {
                    this->mesh.back().set_vertex(slicing.vertex());
                    this->mesh.back().init();
                }

                break;
            }
            default:
                break;
        }
    }
    catch (const runtime_error &error) {
        cerr << error.what() << '\n';
    }

    if (first) cout << "==============================================" << '\n';

    vector<float> histogram(histogram_int.begin(), histogram_int.end());

    return histogram;
}

void generate_combo(map<string, METHOD> &methods, vector<string> &filenames)
{
    // generate methods combo
    methods["Iso Surface"] = METHOD::ISOSURFACE;
    methods["Slicing"] = METHOD::SLICING;

    // generate filenames combo
    DIR *dp;
    dirent *dirp;

    if ((dp = opendir("./Data/Scalar/")) != NULL) {
        while ((dirp = readdir(dp)) != NULL) {
            string temp = dirp->d_name;
            size_t index = temp.find(".inf");

            if (index != string::npos) filenames.push_back(temp.substr(0, index));
        }
    }

    sort(filenames.begin(), filenames.end());
}

void WindowManagement::set_general()
{
    static METHOD current_method = METHOD::NONE;
    static bool first_time = true;

    static string method = "Iso Surface";
    static map<string, METHOD> methods;

    static string filename = "engine";
    static vector<string> filenames;

    static float clip_normal[] = { 0.0, 0.0, 0.0 }, clip_distance = 1.0;

    static vector<float> histogram;

    if (first_time) generate_combo(methods, filenames);
    first_time = false;

    // set Controller position and size
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(WIDTH / 3, HEIGHT / 3), ImGuiCond_Once);

    // Controller window
    ImGui::Begin("Controller");
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
    if (ImGui::Button("Clean")) {
        current_method = METHOD::NONE;
        this->mesh.clear();
        histogram.clear();
    }

    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        current_method = methods[method];
        histogram = this->load(filename, methods[method], true);
    }

    ImGui::SetWindowFontScale(1.0);
    ImGui::SliderFloat3("Clip Plane Normal", clip_normal, -1.0, 1.0);
    ImGui::SliderFloat("Clip Plane Distanse", &clip_distance, -150.0, 150.0);

    ImGui::End();

    if (histogram.size() != 0) {
        ImVec2 histogram_size = ImVec2(WIDTH / 3 - 15, HEIGHT / 3 - 35);
        float max_count = *max_element(histogram.begin(), histogram.end()) + 10;
        // set Histogram position and size
        ImGui::SetNextWindowPos(ImVec2(10, 10 + HEIGHT / 3 + 10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(WIDTH / 3, HEIGHT / 3), ImGuiCond_Once);
        // Histogram window
        ImGui::Begin("Histogram");
        ImGui::PlotHistogram("## Data", histogram.data(), histogram.size(), 0, filename.c_str(), 0.0, max_count, histogram_size);
        ImGui::End();
    }

    glm::vec3 temp = glm::make_vec3(clip_normal);
    if (glm::length(temp) > EPSILON) temp = glm::normalize(temp);
    glm::vec4 clip_plane = glm::vec4(temp, clip_distance);

    if (current_method != METHOD::NONE) {
        this->shader_map[current_method].set_uniform("clip_plane", clip_plane);
        this->shader_map[current_method].set_uniform("view_pos", this->camera.position());
        this->shader_map[current_method].set_uniform("light_pos", this->camera.position());
        this->shader_map[current_method].set_uniform("light_color", glm::vec3(1.0, 1.0, 1.0));
    }
    if (current_method == METHOD::SLICING) this->load(filename, METHOD::SLICING, false);
}

void WindowManagement::init()
{
    glfwSetErrorCallback(WindowManagement::error_callback);

    if (!glfwInit()) throw runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

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
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform / Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init("#version 440");

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debug_callback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->set_callback();

    this->shader_map[METHOD::ISOSURFACE] = Shader(
        "./src/shader/iso_surface/vertex.glsl",
        "./src/shader/iso_surface/fragment.glsl");
    this->shader_map[METHOD::SLICING] = Shader(
        "./src/shader/slicing/vertex.glsl",
        "./src/shader/slicing/fragment.glsl");
}

void WindowManagement::main_loop()
{
    while (!glfwWindowShouldClose(this->window)) {
        glClearColor(0.2, 0.2, 0.2, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // start imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        this->set_general();

        for (size_t i = 0; i < this->mesh.size(); i++) {
            Transformation transformation(this->shader_map[this->mesh[i].method()]);
            transformation.set_projection(WIDTH, HEIGHT, this->rate, 0.0, 500.0);
            transformation.set_view(this->camera.view_matrix());

            this->mesh[i].transform(transformation);
            transformation.set();

            if (this->mesh[i].method() == METHOD::ISOSURFACE) {
                this->shader_map[METHOD::ISOSURFACE].set_uniform("object_color", glm::vec4(0.41, 0.37, 0.89, 1.0));
            }
            this->mesh[i].draw(GL_FILL);
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