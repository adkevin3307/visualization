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
#include "Transformation.h"

using namespace std;

WindowManagement::WindowManagement()
    : last_xpos(0.0), last_ypos(0.0), rate(7.0)
{
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
            distribution[i][j] = 20 * log2(distribution[i][j]);
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

void WindowManagement::load(string filename, METHOD method, bool update, bool custom)
{
    string base = "./Data/";

    if (update == false) cout << "==================== Load ====================" << '\n';

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

                this->mesh.push_back(temp_mesh);

                break;
            }
            case (METHOD::SLICING): {
                static Slicing slicing;

                if (update == false) {
                    cout << "Method: " << "Slicing Method" << '\n';

                    filename = base + "Scalar/" + filename;

                    slicing = Slicing(filename + ".inf", filename + ".raw");
                    slicing.run(this->camera.position());

                    Mesh temp_mesh(slicing, METHOD::SLICING);
                    temp_mesh.enable_texture(2);
                    temp_mesh.init();
                    temp_mesh.init_texture(GL_TEXTURE_2D, 0);
                    temp_mesh.init_texture(GL_TEXTURE_3D, 1);
                    temp_mesh.set_texture(0, slicing.texture_2d(), slicing.texture_2d_shape());
                    temp_mesh.set_texture(1, slicing.texture_3d(), slicing.texture_3d_shape());

                    this->mesh.push_back(temp_mesh);
                }

                if (slicing.run(this->camera.position())) {
                    this->mesh.back().set_vertex(slicing.vertex());
                    this->mesh.back().init();
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

                this->mesh.push_back(border_mesh);

                Mesh temp_mesh(stream_line, METHOD::STREAMLINE);
                temp_mesh.init();

                this->mesh.push_back(temp_mesh);

                break;
            }
            case METHOD::SAMMONMAPPING: {
                cout << "Method: " << "Sammon Mapping" << '\n';

                SammonMapping sammon_mapping;

                if (custom) {
                    filename = base + "Custom/" + filename;
                    sammon_mapping = SammonMapping(filename + ".txt");
                }
                else {
                    filename = base + "Scalar/" + filename;
                    sammon_mapping = SammonMapping(filename + ".inf", filename + ".raw");
                }

                sammon_mapping.run(0.3);

                Mesh temp_mesh(sammon_mapping, METHOD::SAMMONMAPPING);
                temp_mesh.init();

                this->mesh.push_back(temp_mesh);
            }
            default:
                break;
        }
    }
    catch (const runtime_error &error) {
        cerr << error.what() << '\n';
    }

    if (update == false) cout << "==============================================" << '\n';
}

void generate_combo(
    map<string, METHOD> &methods,
    vector<string> &scalar_files,
    vector<string> &custom_files,
    vector<string> &vector_files
)
{
    // generate methods combo
    methods["Iso Surface"] = METHOD::ISOSURFACE;
    methods["Slicing"] = METHOD::SLICING;
    methods["Stream Line"] = METHOD::STREAMLINE;
    methods["Sammon Mapping"] = METHOD::SAMMONMAPPING;

    // generate scalar files combo
    DIR *dp;
    dirent *dirp;

    if ((dp = opendir("./Data/Scalar/")) != NULL) {
        while ((dirp = readdir(dp)) != NULL) {
            string temp = dirp->d_name;
            size_t index = temp.find(".inf");

            if (index != string::npos) scalar_files.push_back(temp.substr(0, index));
        }
    }

    // generate custom files
    custom_files.push_back("iris");
    custom_files.push_back("grade");

    // generate vector files combo
    if ((dp = opendir("./Data/Vector")) != NULL) {
        while ((dirp = readdir(dp)) != NULL) {
            string temp = dirp->d_name;

            if (temp.find("vec") != string::npos) vector_files.push_back(temp);
        }
    }
}

double gaussian(double mu, double sigma, double value)
{
    if (sigma == 0.0) return 0.0;

    double result = exp(-1 * (value - mu) * (value - mu) / (2 * sigma * sigma));

    return result;
}

double gaussian_2d(glm::vec2 mu, glm::vec2 sigma, glm::vec2 value)
{
    if (sigma.x == 0.0 && sigma.y == 0.0) return 0.0;

    double coefficient = sqrt(sigma.x * sigma.y);

    double temp[2];
    for (auto i = 0; i < 2; i++) {
        temp[i] = ((value[i] - mu[i]) * (value[i] - mu[i])) / (2 * sigma[i] * sigma[i]);
    }

    double result = exp(-1 * (temp[0] + temp[1]));

    return min(result / coefficient, 1.0);
}

void WindowManagement::gui()
{
    static METHOD current_method = METHOD::NONE;
    static bool first_time = true;

    static double size = 20.0 * log2(MAX_GRADIENT_MAGNITUDE) + 1.0;

    static string method = "Iso Surface";
    static map<string, METHOD> methods;

    static string scalar_file = "engine";
    static vector<string> scalar_files;

    static string custom_file = "iris";
    static vector<string> custom_files;

    static string vector_file = "1.vec";
    static vector<string> vector_files;

    static int current_data = 0;

    static bool equalization = false;

    static int current_color = 0;

    static float clip_normal[] = { 0.0, 0.0, 0.0 }, clip_distance = 1.0;

    static vector<float> histogram;
    static vector<vector<float>> distribution;

    static vector<vector<float>> color(3, vector<float>(256, 0.0));
    static vector<vector<float>> alpha(256, vector<float>(size, 0.0));

    if (first_time) {
        generate_combo(methods, scalar_files, custom_files, vector_files);
        first_time = false;
    }

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

    if (method != "Sammon Mapping") current_data = 0;
    else {
        // select data
        ImGui::RadioButton("Scalar Data", &current_data, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Custom Data", &current_data, 1);
    }

    if (method == "Iso Surface" || method == "Slicing" || method == "Sammon Mapping") {
        if (current_data == 0) {
            if (ImGui::BeginCombo("## Scalar Files", scalar_file.c_str())) {
                for (size_t i = 0; i < scalar_files.size(); i++) {
                    bool selected = (scalar_file == scalar_files[i]);

                    if (ImGui::Selectable(scalar_files[i].c_str(), selected)) scalar_file = scalar_files[i];
                    if (selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
        else if (current_data == 1) {
            if (ImGui::BeginCombo("## Custom Files", custom_file.c_str())) {
                for (size_t i = 0; i < custom_files.size(); i++) {
                    bool selected = (custom_file == custom_files[i]);

                    if (ImGui::Selectable(custom_files[i].c_str(), selected)) custom_file = custom_files[i];
                    if (selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
    }
    else if (method == "Stream Line") {
        if (ImGui::BeginCombo("## Vector Files", vector_file.c_str())) {
            for (size_t i = 0; i < vector_files.size(); i++) {
                bool selected = (vector_file == vector_files[i]);

                if (ImGui::Selectable(vector_files[i].c_str(), selected)) vector_file = vector_files[i];
                if (selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    if (method != "Stream Line" && method != "Sammon Mapping") {
        ImGui::Checkbox("Equalization", &equalization);

        if (ImGui::Button("Load")) {
            load_volume(scalar_file, histogram, distribution, equalization);
        }
        ImGui::SameLine();
    }

    if (ImGui::Button("Show")) {
        if (method != "Stream Line" && method != "Sammon Mapping") {
            save_transfer_table(scalar_file, color, alpha, equalization);
        }

        current_method = methods[method];
        if (current_method == METHOD::STREAMLINE) {
            this->load(vector_file, current_method, false, false);
        }
        else if (current_method == METHOD::SAMMONMAPPING) {
            if (current_data == 0) {
                // Scalar Data
                this->load(scalar_file, current_method, false, false);
            }
            else if (current_data == 1) {
                // Custom Data
                this->load(custom_file, current_method, false, true);
            }
        }
        else {
            this->load(scalar_file, current_method, false, false);
        }
        
    }
    ImGui::SameLine();

    if (ImGui::Button("Clean")) {
        current_method = METHOD::NONE;

        if (this->mesh.size() != 0) {
            this->mesh.clear();
        }
        else {
            for (size_t i = 0; i < color.size(); i++) {
                fill(color[i].begin(), color[i].end(), 0.0);
            }

            for (size_t i = 0; i < alpha.size(); i++) {
                fill(alpha[i].begin(), alpha[i].end(), 0.0);
            }
        }
    }

    if (method != "Stream Line" && method != "Sammon Mapping") {
        // select color
        ImGui::RadioButton("Red", &current_color, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Green", &current_color, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Blue", &current_color, 2);
    }

    if (method != "Stream Line" && method != "Sammon Mapping") {
        ImGui::SetWindowFontScale(1.0);
        ImGui::SliderFloat3("Clip Plane Normal", clip_normal, -1.0, 1.0);
        ImGui::SliderFloat("Clip Plane Distanse", &clip_distance, -150.0, 150.0);
    }

    ImGui::End();

    if (method == "Stream Line" || method == "Sammon Mapping") {
        histogram.clear();
        distribution.clear();
    }

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
            ImPlot::PlotBars(scalar_file.c_str(), histogram.data(), histogram.size());

            if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0)) {
                ImPlotPoint point = ImPlot::GetPlotMousePos();

                for (auto i = 0; i < 256; i++) {
                    color[current_color][i] += gaussian(point.x, 10.0, i);
                    color[current_color][i] = min(1.0f, color[current_color][i]);
                }
            }

            if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(1)) {
                ImPlotPoint point = ImPlot::GetPlotMousePos();

                for (auto i = 0; i < 256; i++) {
                    color[current_color][i] -= gaussian(point.x, 5.0, i);
                    color[current_color][i] = max(0.0f, color[current_color][i]);
                }
            }

            ImPlot::SetPlotYAxis(1);
            ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(255, 0, 0, 255));
            ImPlot::PlotLine("Red", color[0].data(), color[0].size());
            ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(0, 255, 0, 255));
            ImPlot::PlotLine("Green", color[1].data(), color[1].size());
            ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(0, 0, 255, 255));
            ImPlot::PlotLine("Blue", color[2].data(), color[2].size());

            ImPlot::EndPlot();
        }
        ImPlot::PopStyleColor(4);
        // distribution
        ImPlot::SetNextPlotLimits(0.0, 256.0, 0.0, size, ImGuiCond_Always);
        if (ImPlot::BeginPlot("Distribution", "Intensity", "Gradient Magnitude", plot_size, plot_flag)) {
            if (ImGui::IsWindowFocused() && ImGui::IsMouseDown(0)) {
                ImPlotPoint point = ImPlot::GetPlotMousePos();

                for (size_t i = 0; i < alpha.size(); i++) {
                    for (size_t j = 0; j < alpha[i].size(); j++) {
                        alpha[i][j] += gaussian_2d(
                            glm::vec2(point.x, point.y),
                            glm::vec2(10.0, 10.0),
                            glm::vec2(i, j)
                        );
                        alpha[i][j] = min(1.0f, alpha[i][j]);
                    }
                }
            }

            if (ImGui::IsWindowFocused() && ImGui::IsMouseDown(1)) {
                ImPlotPoint point = ImPlot::GetPlotMousePos();

                for (size_t i = 0; i < alpha.size(); i++) {
                    for (size_t j = 0; j < alpha[i].size(); j++) {
                        alpha[i][j] -= gaussian_2d(
                            glm::vec2(point.x, point.y),
                            glm::vec2(10.0, 10.0),
                            glm::vec2(i, j)
                        );
                        alpha[i][j] = max(0.0f, alpha[i][j]);
                    }
                }
            }

            for (size_t i = 0; i < distribution.size(); i++) {
                for (size_t j = 0; j < distribution[i].size(); j++) {
                    ImVec2 rmin = ImPlot::PlotToPixels(ImPlotPoint(i, j + 1));
                    ImVec2 rmax = ImPlot::PlotToPixels(ImPlotPoint(i + 1, j));

                    float gray = min(255.0f, distribution[i][j]);

                    ImPlot::PushPlotClipRect();

                    ImGui::GetWindowDrawList()->AddRectFilled(rmin, rmax, IM_COL32(gray, gray, gray, 255));

                    if (alpha[i][j] > EPSILON) {
                        int alpha_value = alpha[i][j] * 255;

                        ImGui::GetWindowDrawList()->AddRectFilled(rmin, rmax, IM_COL32(137, 207, 240, alpha_value));
                    }

                    ImPlot::PopPlotClipRect();
                }
            }

            ImPlot::EndPlot();
        }
        ImGui::End();
    }

    if (method != "Stream Line" && method != "Sammon Mapping") {
        glm::vec3 temp = glm::make_vec3(clip_normal);
        if (glm::length(temp) > EPSILON) temp = glm::normalize(temp);
        glm::vec4 clip_plane = glm::vec4(temp, clip_distance);

        if (current_method != METHOD::NONE) {
            this->shader_map[current_method].set_uniform("clip_plane", clip_plane);
            this->shader_map[current_method].set_uniform("view_pos", this->camera.position());
            this->shader_map[current_method].set_uniform("light_pos", this->camera.position());
            this->shader_map[current_method].set_uniform("light_color", glm::vec3(1.0, 1.0, 1.0));
        }
    }

    if (current_method == METHOD::SLICING) {
        this->load(scalar_file, current_method, true, false);
    }
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

        for (size_t i = 0; i < this->mesh.size(); i++) {
            Transformation transformation(this->shader_map[this->mesh[i].method()]);
            transformation.set_projection(WIDTH, HEIGHT, this->rate, 0.0, 500.0);
            transformation.set_view(this->camera.view_matrix());

            this->mesh[i].transform(transformation);
            transformation.set();

            if (this->mesh[i].method() == METHOD::ISOSURFACE) {
                this->shader_map[this->mesh[i].method()].set_uniform("object_color", glm::vec4(0.41, 0.37, 0.89, 1.0));
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