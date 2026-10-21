// throneConverter.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "converter.hpp"
#include "backend.hpp"
#include "pmalloc_win.hpp" 
#include "resource.h"
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void error_argv() {
	std::cout << "usage:\n      throneConverter.exe <throne_link>\n      example:\n      throneConverter.exe \"throne://add/.....\" \n";
	std::cout << " error: arguments\n";
}
ImVec2 v2int32(int32_t _x, int32_t _y) {
    return ImVec2{
        static_cast<float>(_x),
        static_cast<float>(_y)
    };
}
void FontLoadToMemory(void*& ptr, ImGuiIO& io, ImFont*& font, float size_f = 20) {
    HRSRC hRes = FindResourceW(NULL, MAKEINTRESOURCEW(IDR_RCDATA1), RT_RCDATA);
    if (!hRes) return;

    HGLOBAL hData = LoadResource(NULL, hRes);
    if (!hData) return;

    void* pData = LockResource(hData);
    DWORD size = SizeofResource(NULL, hRes);
    if (!pData || size == 0) return;

    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    std::cout << "FontLoadToMemory() - > [HRSRC:" << hRes << "] [HGLOBAL:" << hData << "] [pData : " << pData << "] [size: " << size << "]\n";

    font = io.Fonts->AddFontFromMemoryTTF(pData, static_cast<int>(size), size_f, &font_cfg, io.Fonts->GetGlyphRangesCyrillic());
    ptr = pData;
}
auto g_Throne = pmalloc::pm_new<window>();
const char* getID() {
    auto fnXr = [&](uint64_t u64S) -> uint64_t {
        uint64_t u64Sa = u64S + u64S;
        u64Sa ^= u64Sa >> 33;
        u64Sa ^= u64Sa >> 32;
        u64Sa ^= u64Sa << 8;
        return u64Sa;
        };

    static char szData[9];
    for (int32_t i32A = 0; i32A < 8; ++i32A) {
        szData[i32A] = "abcdef123456789"[fnXr(21102621233ULL) % 15];
    }
    szData[8] = '\0';
    return szData;
}
int main(int argc, char** argv)
{
    g_Throne->SetSize(400, 700);
    g_Throne->setTitleText(L"ThroneConvertorGUI by HCPP20334");
    g_Throne->createWindow(WndProc);
    g_Throne->EnableCustomRender(true, 0, 2);
    g_Throne->initDeviceGL("#version 330");
    g_Throne->initIO(&g_Throne->io);
    g_Throne->SetDarkStyle();
    // load fonts
    void* pFont = nullptr;
    ImFont* font20 = nullptr;

    FontLoadToMemory(pFont, g_Throne->io, font20);
    if (!pFont) {
        MessageBox(g_Throne->hwnd, L"Error: Font Ptr = nullptr! Aborting!!", L"ThroneConvertorGUI::Memory", 1);
    }
    else {
        std::cout << "(pRes) pFont loaded 0x" << pFont << " -> 0x" << &pFont << "\n";
    }
    void* pFont40 = nullptr;
    ImFont* font40 = nullptr;

    FontLoadToMemory(pFont40, g_Throne->io, font40,40);
    if (!pFont40) {
        MessageBox(g_Throne->hwnd, L"Error: Font Ptr = nullptr! Aborting!!", L"ThroneConvertorGUI::Memory", 1);
    }
    else {
        std::cout << "(pRes) pFont loaded 0x" << pFont40 << " -> 0x" << &pFont40 << "\n";
    }
    void* pFont10 = nullptr;
    ImFont* font10 = nullptr;
    FontLoadToMemory(pFont10, g_Throne->io, font10, 14);
    bool g_bApp = false;
    if (!pFont10) {
        MessageBox(g_Throne->hwnd, L"Error: Font Ptr = nullptr! Aborting!!", L"ThroneConvertorGUI::Memory", 1);
    }
    else {
        std::cout << "(pRes) pFont loaded 0x" << pFont10 << " -> 0x" << &pFont10 << "\n";
    }
    //
    std::string g_sThroneLinkBuffer = "", g_sThroneOutLinkBuffer = "";
    //
    bool bIsRawJson = false;
    while (g_Throne->render) {
        g_Throne->NewFrame();
        g_Throne->getMessage();
        g_Throne->CPUDelay(1);
        ImGui::Begin("#app", &g_bApp, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::SetWindowPos({ 0,0 });
        ImGui::SetWindowSize(v2int32(g_Throne->g_Width, g_Throne->g_Height));
        auto align_p = [&]() {
            ImGui::SetCursorPosX(30);
        };
        ImGui::SetCursorPos({ 30,30 });
        ImGui::PushFont(font40);
        ImGui::Text("ThroneConverter");
        ImGui::PopFont();
        ImGui::SetCursorPos({ 30,130 });
        ImGui::Text("%s", (bIsRawJson ? "Сырой JSON          " : "Ссылка throne://add/")); ImGui::SameLine();
        ImGui::SetCursorPosY((ImGui::GetCursorPosY())); ImGui::Checkbox("RAW JSON", &bIsRawJson);
        align_p();
        ImGui::PushID('aa1');
        ImGui::InputTextMultiline("", &g_sThroneLinkBuffer, v2int32(g_Throne->g_Width - 60, 150));
        ImGui::PopID();
        align_p();
        ImGui::Text("Вывод");
        ImGui::SameLine();
        ImGui::SetCursorPosY((ImGui::GetCursorPosY()));
        if (ImGui::Button("copy", { 100,30 })) {
            ImGui::SetClipboardText(g_sThroneOutLinkBuffer.c_str());
        }
        ImGui::PushID('aa');
        align_p();
        ImGui::InputTextMultiline("", &g_sThroneOutLinkBuffer, v2int32(g_Throne->g_Width - 60, 150));
        ImGui::PopID();
        align_p();
        if (ImGui::Button("Конвертировать", v2int32(g_Throne->g_Width - 60, 60))) {
            g_sThroneOutLinkBuffer = convertAll(g_sThroneLinkBuffer, bIsRawJson);
        }
        align_p();
        ImGui::PushFont(font10);
        ImGui::Text("v0.5 (build 8fcca254e5ff0300f731b3152c157e39) ");
        ImGui::PopFont();
        ImGui::End();
        g_Throne->viewport();
    }
    pmalloc::pm_delete(g_Throne);
    g_Throne->shutdownDevice();
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED) {
            g_Throne->g_Width = LOWORD(lParam);
            g_Throne->g_Height = HIWORD(lParam);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        exit(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
