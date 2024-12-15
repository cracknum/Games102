// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>
#include <iostream>
#include <vector>
#include <math.h>
#define GL_SILENCE_DEPRECATION

using ImVec2Vector = std::vector<ImVec2>;

void RenderWindow(GLFWwindow *window, const ImVec4 &clear_color);
void DrawCircle(ImGuiIO& io);
void CalculateYanghuiTriangle(int level, std::vector<std::vector<int>>& levelValues);
void CalculateBParameters(int level, std::vector<int>& levelValues);
void CalculateCParameters(int level, std::vector<int>& levelValues);
void CalculateBezierParam(const std::vector<int> &aParam, const std::vector<int> &bParam,const std::vector<int> &cParam, const ImVec2Vector & controlPoints, ImVec2Vector& resultPoints);

void DrawBezier(ImGuiIO& io, const std::vector<std::vector<int>>& aParameters);
void NewFrame();
#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool drawCircle = false;
    bool drawBezier = false;
    int level = 50;
    std::vector<std::vector<int>> AParameters;
    CalculateYanghuiTriangle(level, AParameters);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        NewFrame();
        ImGui::SetNextWindowSize(ImVec2(1280, 720));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("mouse interaction", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImGui::Checkbox("draw circle", &drawCircle);
        ImGui::Checkbox("draw bezier", &drawBezier);

        if (drawCircle)
        {
            DrawCircle(io);
        }

        if (drawBezier)
        {
            // 公式推导参照https://blog.csdn.net/sinat_35676815/article/details/120884682
            DrawBezier(io, AParameters);
        }

        ImGui::End();


        RenderWindow(window, ImVec4(0.45f, 0.55f, 0.60f, 1.00f));


    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void RenderWindow(GLFWwindow *window, const ImVec4 &clear_color) {// Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void DrawCircle(ImGuiIO& io)
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    drawList->AddCircleFilled(io.MousePos, 10.0f, ImGui::GetColorU32(ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f))));
}


void CalculateYanghuiTriangle(int level, std::vector<std::vector<int>>& levelValues)
{
    levelValues.push_back({1});
    std::cout << 1 << std::endl;
    levelValues.push_back({1, 1});
    std::cout << 1 << ", " << 1 << std::endl;
    for (int i = 2; i < level; ++i) {
        std::vector<int> levelValue;
        for (int j = 0; j <= i; ++j) {
            if (0 == j || j == i)
            {
                levelValue.push_back(1);
                std::cout << 1 << ", ";
            }
            else
            {
                auto& preLevelValue = levelValues.at(i - 1);
                levelValue.push_back(preLevelValue[j - 1] + preLevelValue[j]);
                std::cout  << preLevelValue[j - 1]  + preLevelValue[j] << ", ";
            }
        }
        std::cout << std::endl;
        levelValues.push_back(std::move(levelValue));
    }
}

void test()
{

}
ImVec2Vector mousePoints;
void DrawBezier(ImGuiIO& io, const std::vector<std::vector<int>>& aParameters)
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        mousePoints.push_back(io.MousePos);
    }

    if (mousePoints.size() <= 0)
    {
        return;
    }
    std::vector<int> bParameters;
    std::vector<int> cParameters;
    int pointsSize = (int)mousePoints.size();
    CalculateBParameters(pointsSize, bParameters);
    CalculateCParameters(pointsSize, cParameters);

    ImVec2Vector resultVector;

    CalculateBezierParam(aParameters[mousePoints.size() - 1], bParameters, cParameters, mousePoints, resultVector);

    ImDrawList* drawList =  ImGui::GetWindowDrawList();

    for (int i = 1; i <resultVector.size(); ++i) {
        drawList->AddLine(ImVec2(resultVector[i-1]), ImVec2(resultVector[i]), ImGui::GetColorU32(ImGui::ColorConvertFloat4ToU32(ImVec4(0.5f, 0.5f, 0.5f, 1.0f))));
    }

}

void CalculateBParameters(int level, std::vector<int>& bParameters)
{
    for (int i = level - 1; i >= 0; --i)
    {
        bParameters.push_back(i);
    }
}

void CalculateCParameters(int level, std::vector<int>& cParameters)
{
    for (int i = 0; i < level; ++i)
    {
        cParameters.push_back(i);
    }
}

int numberOfPoints = 100;
void CalculateBezierParam(const std::vector<int> &aParam, const std::vector<int> &bParam, const std::vector<int> &cParam, const ImVec2Vector & controlPoints, ImVec2Vector& resultPoints)
{

    for (int i = 0; i < numberOfPoints; ++i) {
        float t = i * 1.0f / numberOfPoints;
        ImVec2 pointInT = {0, 0};
        for (int j = 0; j < controlPoints.size(); ++j) {
            pointInT.x += aParam[j] * std::pow(1 - t, bParam[j]) * std::pow(t, cParam[j]) * controlPoints[j].x;
            pointInT.y += aParam[j] * std::pow(1 - t, bParam[j]) * std::pow(t, cParam[j]) * controlPoints[j].y;
        }

        resultPoints.push_back(pointInT);
    }
}
