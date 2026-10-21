#pragma once
#define CRT_NO_WARNINGS 1
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "render.hpp"
#include <string>
#define max_color_float float(255.0f)
struct screen
{
    int w, h;
    screen() : w(0), h(0) {}
    screen(int w, int h) : w(w), h(h) {}
};
ImVec4 RGBA(float r, float g, float b, float a) {
    return ImVec4{
        r / max_color_float,
        g / max_color_float,
        b / max_color_float,
        a / max_color_float
    };
}
//namespace ImGui {
//    bool InputInt64(const char* label, int64_t* v, int64_t step, int64_t step_fast, ImGuiInputTextFlags flags)
//    {
//        // Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
//        const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
//        return InputScalar(label, ImGuiDataType_S64, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
//    }
//}

struct window {
    HWND hwnd;
    struct WGL_WindowData { HDC hDC; };
    bool bIsMultiThread = false;
    bool render = true;
    LPCWSTR wIcon;
    int64_t fwop = 0;
    HGLRC g_hRC;
    WGL_WindowData g_MainWindow;
    int g_Width;
    int g_Height;
    int gl_minor = 0;
    int gl_major = 3; 
    bool bUseCustomRender = false;
    ImVec4 clear_color = ImVec4(0.01f, 0.01f, 0.02f, 0.00f);
    std::wstring WindowTitle = L"SWEET BREADS (PahomEngineGL)";
    ImGuiIO io;
    void initIO(ImGuiIO* io_ptr) {
        *io_ptr = ImGui::GetIO();
    }
    // Вспомогательные функции
    void SetResizeWindow(int32_t x, int32_t y) {
        RECT r;
        GetWindowRect(hwnd, &r);
        MoveWindow(hwnd, r.left, r.top, x, y, TRUE);//
    }
    void CPUDelay(int64_t i64Delay) {
        auto start = std::chrono::steady_clock::now();
        auto awake_time = start + std::chrono::microseconds(i64Delay);
        std::this_thread::sleep_until(awake_time);
        auto end = std::chrono::steady_clock::now();
    }
    ImVec2 GetWindowSize(HWND HwNd)
    {
        RECT fSizeWindow;
        GetWindowRect(HwNd, &fSizeWindow);
        int64_t fWctx = fSizeWindow.right - fSizeWindow.left;
        int64_t fWcty = fSizeWindow.bottom - fSizeWindow.top;
        return ImVec2(
            static_cast<float>(fWctx), static_cast<float>(fWcty)
        );
    }
    void SetDarkStyle() {
        auto hex = [&](std::string_view hex) {
            if (!hex.empty() && hex[0] == '#')
                hex.remove_prefix(1);
            auto parse = [&](size_t pos, uint8_t def) -> float {
                if (pos + 2 > hex.size())
                    return def / 255.0f;
                unsigned value = def;
                std::from_chars(hex.data() + pos, hex.data() + pos + 2, value, 16);
                return value / 255.0f;
                };
            return ImVec4{
                parse(0, 0),
                parse(2, 0),
                parse(4, 0),
                parse(6, 255)
            };
        };
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = RGBA(18, 18, 25, 255);
        colors[ImGuiCol_ChildBg] = RGBA(33, 33, 35, 0.416);
        colors[ImGuiCol_PopupBg] = RGBA(5, 5, 7, 255);
        colors[ImGuiCol_Border] = RGBA(35, 35, 55, 255);
        colors[ImGuiCol_BorderShadow] = RGBA(60, 60, 70, 1);
        colors[ImGuiCol_FrameBg] = RGBA(33, 33, 35, 0.816);
        colors[ImGuiCol_FrameBgHovered] = RGBA(5, 5, 5, 255);
        colors[ImGuiCol_FrameBgActive] = RGBA(5, 5, 5, 255);
        colors[ImGuiCol_TitleBg] = ImVec4(0.353f, 0.157f, 1.000f, 1.000f);
        colors[ImGuiCol_TitleBgActive] = RGBA(135, 165, 255, 155);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.60f, 1.0f, 1.0f);
        colors[ImGuiCol_SliderGrab] = RGBA(35, 35, 55, 255);
        colors[ImGuiCol_SliderGrabActive] = RGBA(35, 35, 55, 255);
        colors[ImGuiCol_Button] = RGBA(160, 100, 100, 255);
        colors[ImGuiCol_ButtonHovered] = RGBA(35, 35, 55, 255);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_Header] = RGBA(5, 5, 5, 255);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.15f, 0.15f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
        colors[ImGuiCol_Separator] = RGBA(35, 35, 55, 255);
        colors[ImGuiCol_SeparatorHovered] = RGBA(35, 35, 55, 255);
        colors[ImGuiCol_SeparatorActive] = RGBA(35, 35, 55, 255);
        colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.13f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.26f, 0.98f, 0.50f);
        colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.20f, 0.58f, 0.73f);
        //colors[ImGuiCol_TabSelected] = ImVec4(0.29f, 0.20f, 0.68f, 1.00f);
      ///  colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        //colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
      //  colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
        //colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
        colors[ImGuiCol_PlotLines] = RGBA(0, 235, 147, 255);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = RGBA(35, 35, 55, 255);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = RGBA(33, 33, 35, 0.816);
        //colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = RGBA(33, 33, 35, 0.816);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        // colors[ImGuiCol_NavCursor] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = RGBA(33, 33, 35, 0.816);
        colors[ImGuiCol_ModalWindowDimBg] = RGBA(33, 33, 35, 0.816);
        style.FrameBorderSize = 1;
        style.WindowBorderSize = 0;
    }
    bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data) {
        HDC hDc = ::GetDC(hWnd);
        PIXELFORMATDESCRIPTOR pfd = { 0 };
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1; // Исправлено с 3
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cAlphaBits = 8;
        pfd.cColorBits = 32;
        const int pf = ::ChoosePixelFormat(hDc, &pfd);
        if (pf == 0) return false;
        if (::SetPixelFormat(hDc, pf, &pfd) == FALSE) return false;
        ::ReleaseDC(hWnd, hDc);

        data->hDC = ::GetDC(hWnd);
        if (!g_hRC)
            g_hRC = bUseCustomRender ? CreateGL(data->hDC, gl_major, gl_minor) : wglCreateContext(data->hDC); // Используем стандартный или твой CreateGL
        std::cout << std::format("(createDeviceWGL()) use_custom_render={} OpenGL{}.{}\n", bUseCustomRender, gl_major, gl_minor);
        return true;
    }
    void EnableCustomRender(bool v, int minor, int major) {
        bUseCustomRender = v;
        gl_major = major;
        gl_minor = minor;
    }
    void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data) {
        wglMakeCurrent(nullptr, nullptr);
        ::ReleaseDC(hWnd, data->hDC);
    }
    void setTitleText(std::wstring text) {
        WindowTitle = text;
    }
    void SetSize(int width, int height) {
        g_Width = width;
        g_Height = height;
    }
    void getMessage() {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
            {
                render = false;
                
            }
        }
    }
    void LoadResIcon(int id) {
        wIcon = MAKEINTRESOURCEW(id);
    }
    void createWindow(LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)) {
        ImGui_ImplWin32_EnableDpiAwareness();
        WNDCLASSEXW wc = { sizeof(wc), CS_OWNDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"PahomEngine", nullptr };
        //wc.hIcon = LoadIconW(wc.hInstance, wIcon);
        ::RegisterClassExW(&wc);
        hwnd = ::CreateWindowW(wc.lpszClassName, WindowTitle.data(), WS_OVERLAPPEDWINDOW, 100, 80, g_Width,g_Height, nullptr, nullptr, wc.hInstance, nullptr);
        ImGui_ImplWin32_EnableDpiAwareness();
        ::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX);
        if (!CreateDeviceWGL(hwnd, &g_MainWindow)) {
            exit(1);
        }
        ::ShowWindow(hwnd, SW_SHOW);
        ::UpdateWindow(hwnd);
    }

    void initDeviceGL(const char* ccOGLver = "#version 330") {
        wglMakeCurrent(g_MainWindow.hDC, g_hRC);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplWin32_InitForOpenGL(hwnd);
        ImGui_ImplOpenGL3_Init(ccOGLver);
    }

    void NewFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void viewport() {
        glViewport(0, 0, g_Width,g_Height);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        ::SwapBuffers(g_MainWindow.hDC);
    }

    void shutdownDevice() {
        std::cout << " (PahomEngine) shutDown Device (WIN32) (OpenGL)" << std::endl;
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        CleanupDeviceWGL(hwnd, &g_MainWindow);
        if (g_hRC) wglDeleteContext(g_hRC);
        ::DestroyWindow(hwnd);
    }
  //////////////////////////////////
};