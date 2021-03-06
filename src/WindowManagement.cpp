#include "WindowManagement.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <implot/implot.h>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <dirent.h>
#include <cmath>
#include <fstream>

#include "Method.h"
#include "IsoSurface.h"
#include "Slicing.h"
#include "StreamLine.h"
#include "SammonMapping.h"
#include "Mesh.h"
#include "MeshManagement.h"

using namespace std;

WindowManagement::WindowManagement()
    : files_index(0), last_xpos(0.0), last_ypos(0.0), rate(7.0)
{
    this->files.resize(3);
    this->generate_combo();
}

WindowManagement::~WindowManagement()
{
}

void APIENTRY debug_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    const void *userParam
)
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
        case GLFW_KEY_LEFT_CONTROL: case GLFW_KEY_RIGHT_CONTROL:
            ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
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

void load_volume(string filename, vector<float> &histogram, vector<vector<float>> &distribution, bool equalization)
{
    string inf_file = "./Data/Scalar/" + filename + ".inf", raw_file = "./Data/Scalar/" + filename + ".raw";

    Volume volume(inf_file, raw_file);

    cout << "==================== Info ====================" << '\n';
    volume.show();
    cout << "==============================================" << '\n';

    if (equalization) volume.equalization();

    histogram = volume.histogram();
    distribution = volume.distribution(MAX_GRADIENT_MAGNITUDE);

    for (size_t i = 0; i < distribution.size(); i++) {
        for (size_t j = 0; j < distribution[i].size(); j++) {
            distribution[i][j] = min(255.0f, 20 * log2(distribution[i][j]));
        }
    }
}

void save_transfer_table(string filename, vector<vector<float>> &color, vector<vector<float>> &alpha, bool equalization)
{
    cout << "==================== Save ====================" << '\n';

    fstream file;
    file.open("transfer_function.txt", ios::trunc | ios::out);

    cout << "filename: " << filename << '\n';
    file << filename << '\n';

    cout << "equalization: " << (equalization ? "True" : "False") << '\n';
    file << equalization << '\n';

    cout << "table size: " << alpha.size() << ", " << alpha[0].size() << '\n';
    file << alpha.size() << ' ' << alpha[0].size() << '\n';
    for (size_t i = 0; i < alpha.size(); i++) {
        for (size_t j = 0; j < alpha[i].size(); j++) {
            file << color[0][i] << ' ' << color[1][i] << ' ' << color[2][i] << ' ';
            file << alpha[i][j] << (j == alpha[i].size() - 1 ? '\n' : ' ');
        }
    }

    file.close();

    cout << "==============================================" << '\n';
}

void WindowManagement::load(string filename, METHOD method)
{
    string base = "./Data/";

    cout << "==================== Load ====================" << '\n';

    this->shader_map[method].use();

    try {
        switch (method) {
            case (METHOD::ISOSURFACE): {
                cout << "Method: " << "Iso Surface" << '\n';

                filename = base + "Scalar/" + filename;

                IsoSurface iso_surface(filename + ".inf", filename + ".raw");
                iso_surface.run();

                Mesh temp_mesh(iso_surface, METHOD::ISOSURFACE);
                temp_mesh.init();

                MeshManagement::add(temp_mesh);

                break;
            }
            case (METHOD::SLICING): {
                cout << "Method: " << "Slicing Method" << '\n';

                filename = base + "Scalar/" + filename;

                Slicing slicing(filename + ".inf", filename + ".raw");
                for (auto i = 0; i < 6; i++) {
                    slicing.run(i);

                    Mesh temp_mesh(slicing, METHOD::SLICING);
                    temp_mesh.enable_texture(2);
                    temp_mesh.init();
                    temp_mesh.init_texture(GL_TEXTURE_2D, 0);
                    temp_mesh.init_texture(GL_TEXTURE_3D, 1);
                    temp_mesh.set_texture(0, slicing.texture_2d(), slicing.texture_2d_shape());
                    temp_mesh.set_texture(1, slicing.texture_3d(), slicing.texture_3d_shape());

                    MeshManagement::add(temp_mesh);
                }

                break;
            }
            case (METHOD::STREAMLINE): {
                cout << "Method: " << "Stream Line" << '\n';

                filename = base + "Vector/" + filename;

                StreamLine stream_line(filename);
                stream_line.run();

                int border_size = stream_line.base_scale() << stream_line.max_scale();
                cout << "border size: " << border_size << '\n';
                glm::vec3 shape = stream_line.shape();

                vector<GLfloat> border;
                for (auto i = 0; i < border_size + 1; i++) {
                    for (auto j = 0; j < 2; j++) {
                        border.push_back(i / (float)border_size * shape.x);
                        border.push_back(j * shape.y);

                        border.push_back(0.41);
                        border.push_back(0.37);
                        border.push_back(0.89);
                        border.push_back(0.20);

                        border.push_back(1);
                    }

                    for (auto j = 0; j < 2; j++) {
                        border.push_back(j * shape.x);
                        border.push_back(i / (float)border_size * shape.y);

                        border.push_back(0.41);
                        border.push_back(0.37);
                        border.push_back(0.89);
                        border.push_back(0.20);

                        border.push_back(1);
                    }
                }

                Mesh border_mesh(border, vector<int>{2, 4, 1}, GL_LINES, shape, METHOD::STREAMLINE);
                border_mesh.init();

                MeshManagement::add(border_mesh);

                Mesh temp_mesh(stream_line, METHOD::STREAMLINE);
                temp_mesh.init();

                MeshManagement::add(temp_mesh);

                break;
            }
            case METHOD::SAMMONMAPPING: {
                cout << "Method: " << "Sammon Mapping" << '\n';

                SammonMapping sammon_mapping;

                if (this->files_index == 0) {
                    filename = base + "Scalar/" + filename;
                    sammon_mapping = SammonMapping(filename + ".inf", filename + ".raw");
                }
                if (this->files_index == 2) {
                    filename = base + "Custom/" + filename;
                    sammon_mapping = SammonMapping(filename + ".txt");
                }

                sammon_mapping.run();

                Mesh temp_mesh(sammon_mapping, METHOD::SAMMONMAPPING);
                temp_mesh.init();

                MeshManagement::add(temp_mesh);

                break;
            }
            default:
                break;
        }
    }
    catch (const runtime_error &error) {
        cerr << error.what() << '\n';
    }

    cout << "==============================================" << '\n';
}

void WindowManagement::generate_combo()
{
    // generate methods combo
    this->methods["Iso Surface"] = METHOD::ISOSURFACE;
    this->methods["Slicing Method"] = METHOD::SLICING;
    this->methods["Stream Line"] = METHOD::STREAMLINE;
    this->methods["Sammon Mapping"] = METHOD::SAMMONMAPPING;

    // generate scalar files combo
    DIR *dp;
    dirent *dirp;

    if ((dp = opendir("./Data/Scalar/")) != NULL) {
        while ((dirp = readdir(dp)) != NULL) {
            string temp = dirp->d_name;
            size_t index = temp.find(".inf");

            if (index != string::npos) this->files[0].push_back(temp.substr(0, index));
        }
    }

    // generate vector files combo
    if ((dp = opendir("./Data/Vector")) != NULL) {
        while ((dirp = readdir(dp)) != NULL) {
            string temp = dirp->d_name;

            if (temp.find("vec") != string::npos) this->files[1].push_back(temp);
        }
    }

    // generate custom files
    this->files[2].push_back("iris");
    this->files[2].push_back("grade");
}

double gaussian(double mu, double sigma, double value)
{
    if (sigma == 0.0) return 0.0;

    double result = exp(-1 * (value - mu) * (value - mu) / (2 * sigma * sigma));

    return (result < EPSILON ? 0.0 : result);
}

double gaussian_2d(glm::vec2 mu, glm::vec2 sigma, glm::vec2 value)
{
    if (sigma.x == 0.0 && sigma.y == 0.0) return 0.0;

    double coefficient = sqrt(sigma.x * sigma.y);
    glm::vec2 temp = ((value - mu) * (value - mu)) / (2.0f * sigma * sigma);
    double result = min(exp(-1 * (temp[0] + temp[1])) / coefficient, 1.0);

    return (result < EPSILON ? 0.0 : result);
}

void WindowManagement::gui()
{
    static bool loaded = false;
    static bool showed = false;

    static METHOD current_method = METHOD::NONE;

    static double size = 20.0 * log2(MAX_GRADIENT_MAGNITUDE) + 1.0;

    static string method = this->methods.begin()->first;
    static string selected_file = this->files[this->files_index][0];

    static bool equalization = false;

    static int current_color = 0;

    static float clip_normal[] = { 0.0, 0.0, 0.0 }, clip_distance = 1.0;

    static vector<float> histogram;
    static vector<vector<float>> distribution;

    static vector<vector<float>> color(3, vector<float>(256, 0.0));
    static vector<vector<float>> alpha(256, vector<float>(size, 0.0));

    // set Controller position and size
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(WIDTH / 3, HEIGHT / 3), ImGuiCond_Once);
    // Controller window
    ImGui::Begin("Controller");
    ImGui::SetWindowFontScale(1.2);

    if (ImGui::BeginCombo("## Methods", method.c_str())) {
        string old_method = method;

        for (auto it = this->methods.begin(); it != this->methods.end(); it++) {
            bool selected = (method == it->first);

            if (ImGui::Selectable(it->first.c_str(), selected)) method = it->first;
            if (selected) ImGui::SetItemDefaultFocus();
        }

        if (old_method != method) {
            this->files_index = (method == "Stream Line");

            selected_file = this->files[this->files_index][0];
        }
        ImGui::EndCombo();
    }

    if (method == "Sammon Mapping") {
        // select data
        if (ImGui::RadioButton("Scalar Data", &(this->files_index), 0)) {
            selected_file = this->files[this->files_index][0];
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Custom Data", &(this->files_index), 2)) {
            selected_file = this->files[this->files_index][0];
        }
    }

    if (ImGui::BeginCombo("## Files", selected_file.c_str())) {
        for (auto it = this->files[this->files_index].begin(); it != this->files[this->files_index].end(); it++) {
            bool selected = (selected_file == *it);

            if (ImGui::Selectable((*it).c_str(), selected)) selected_file = *it;
            if (selected) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    if (method == "Slicing Method") {
        ImGui::Checkbox("Equalization", &equalization);

        ImGui::RadioButton("Red", &current_color, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Green", &current_color, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Blue", &current_color, 2);

        if (!loaded && ImGui::Button("Load")) {
            loaded = true;

            load_volume(selected_file, histogram, distribution, equalization);
        }
    }

    if ((loaded || method != "Slicing Method") && ImGui::Button("Show")) {
        showed = true;

        if (method == "Slicing Method") {
            save_transfer_table(selected_file, color, alpha, equalization);
        }

        current_method = this->methods[method];
        this->load(selected_file, current_method);
    }

    ImGui::SameLine();
    if (ImGui::Button("Clean")) {
        current_method = METHOD::NONE;

        if (showed) {
            showed = false;

            MeshManagement::clear();
        }
        else if (loaded) {
            loaded = false;

            for (size_t i = 0; i < color.size(); i++) {
                fill(color[i].begin(), color[i].end(), 0.0);
            }

            for (size_t i = 0; i < alpha.size(); i++) {
                fill(alpha[i].begin(), alpha[i].end(), 0.0);
            }

            histogram.clear();
            distribution.clear();
        }
    }

    if (method == "Iso Surface") {
        ImGui::SetWindowFontScale(1.0);
        ImGui::SliderFloat3("Clip Plane Normal", clip_normal, -1.0, 1.0);
        ImGui::SliderFloat("Clip Plane Distanse", &clip_distance, -150.0, 150.0);
    }

    ImGui::End();

    if (histogram.size() != 0) {
        float max_amount = *max_element(histogram.begin(), histogram.end()) + 10;

        ImVec2 plot_size(-1, 0);
        ImPlotFlags plot_flag = ImPlotFlags_MousePos | ImPlotFlags_Legend | ImPlotFlags_Highlight | ImPlotFlags_CullData;

        // set Information position and size
        ImGui::SetNextWindowPos(ImVec2(WIDTH - (WIDTH / 3) - 10, 10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(WIDTH / 3, (HEIGHT / 3) * 2 + 40), ImGuiCond_Once);

        ImGui::Begin("Information");
        // histogram
        ImPlot::SetNextPlotLimits(0.0, 256.0, 0.0, max_amount, ImGuiCond_Always);
        ImPlot::SetNextPlotLimitsY(0.0, 1.1, ImGuiCond_Always, 1);
        if (ImPlot::BeginPlot("## RGB", "Intensity", "Amount", plot_size, plot_flag)) {
            ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(128, 128, 128, 255));
            ImPlot::PlotBars(selected_file.c_str(), histogram.data(), histogram.size());

            if (ImPlot::IsPlotHovered() && ImGui::IsWindowFocused() && ImGui::IsAnyMouseDown()) {
                ImPlotPoint point = ImPlot::GetPlotMousePos();

                double delta, sigma;
                if (ImGui::IsMouseDown(0)) {
                    delta = 1.0;
                    sigma = 10.0;
                }
                if (ImGui::IsMouseDown(1)) {
                    delta = -1.0;
                    sigma = 5.0;
                }

                for (auto i = 0; i < 256; i++) {
                    color[current_color][i] += gaussian(point.x, sigma, i) * delta;
                    color[current_color][i] = min(1.0f, max(0.0f, color[current_color][i]));
                }

            }

            ImPlot::SetPlotYAxis(1);
            ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(255, 0, 0, 255));
            ImPlot::PlotLine("Red", color[0].data(), color[0].size());
            ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(0, 255, 0, 255));
            ImPlot::PlotLine("Green", color[1].data(), color[1].size());
            ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(0, 0, 255, 255));
            ImPlot::PlotLine("Blue", color[2].data(), color[2].size());

            ImPlot::PopStyleColor(4);
            ImPlot::EndPlot();
        }
        // distribution
        ImPlot::SetNextPlotLimits(0.0, 256.0, 0.0, size, ImGuiCond_Always);
        if (ImPlot::BeginPlot("Distribution", "Intensity", "Gradient Magnitude", plot_size, plot_flag)) {
            if (ImGui::IsWindowFocused() && ImGui::IsAnyMouseDown()) {
                ImPlotPoint point = ImPlot::GetPlotMousePos();

                double delta;
                if (ImGui::IsMouseDown(0)) delta = 1.0;
                if (ImGui::IsMouseDown(1)) delta = -1.0;

                for (size_t i = 0; i < alpha.size(); i++) {
                    for (size_t j = 0; j < alpha[i].size(); j++) {
                        alpha[i][j] += gaussian_2d(
                            glm::vec2(point.x, point.y),
                            glm::vec2(10.0, 10.0),
                            glm::vec2(i, j)
                        ) * delta;
                        alpha[i][j] = min(1.0f, max(0.0f, alpha[i][j]));
                    }
                }
            }

            for (size_t i = 0; i < distribution.size(); i++) {
                for (size_t j = 0; j < distribution[i].size(); j++) {
                    ImVec2 rmin = ImPlot::PlotToPixels(ImPlotPoint(i, j + 1));
                    ImVec2 rmax = ImPlot::PlotToPixels(ImPlotPoint(i + 1, j));

                    int gray = distribution[i][j];
                    ImPlot::PushPlotClipRect();
                    ImGui::GetWindowDrawList()->AddRectFilled(rmin, rmax, IM_COL32(gray, gray, gray, 255));
                    ImPlot::PopPlotClipRect();

                    if (alpha[i][j] >= EPSILON) {
                        ImPlot::PushPlotClipRect();
                        ImGui::GetWindowDrawList()->AddRectFilled(rmin, rmax, IM_COL32(137, 207, 240, alpha[i][j] * 255));
                        ImPlot::PopPlotClipRect();
                    }
                }
            }

            ImPlot::EndPlot();
        }
        ImGui::End();
    }

    if (current_method == METHOD::ISOSURFACE) {
        glm::vec3 temp = glm::make_vec3(clip_normal);
        if (glm::length(temp) > EPSILON) temp = glm::normalize(temp);
        glm::vec4 clip_plane = glm::vec4(temp, clip_distance);

        this->shader_map[current_method].set_uniform("clip_plane", clip_plane);
    }

    if (current_method == METHOD::ISOSURFACE || current_method == METHOD::SLICING) {
        this->shader_map[current_method].set_uniform("view_pos", this->camera.position());
        this->shader_map[current_method].set_uniform("light_pos", this->camera.position());
        this->shader_map[current_method].set_uniform("light_color", glm::vec3(1.0, 1.0, 1.0));
    }

    MeshManagement::update(this->camera.position(), current_method);
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

    glEnable(GL_PROGRAM_POINT_SIZE);

    this->set_callback();

    cout << "=================== Shader ===================" << '\n';

    this->shader_map[METHOD::ISOSURFACE] = Shader(
        "./src/shader/iso_surface/vertex.glsl",
        "./src/shader/iso_surface/fragment.glsl"
    );
    this->shader_map[METHOD::SLICING] = Shader(
        "./src/shader/slicing/vertex.glsl",
        "./src/shader/slicing/fragment.glsl"
    );
    this->shader_map[METHOD::STREAMLINE] = Shader(
        "./src/shader/stream_line/vertex.glsl",
        "./src/shader/stream_line/fragment.glsl"
    );
    this->shader_map[METHOD::SAMMONMAPPING] = Shader(
        "./src/shader/sammon_mapping/vertex.glsl",
        "./src/shader/sammon_mapping/fragment.glsl"
    );

    cout << "==============================================" << '\n';
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

        this->gui();
        MeshManagement::draw(this->shader_map, this->camera.view_matrix(), this->rate);

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