#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "common.hpp"
#include <d3d11.h>
#include <random>
#include <dwmapi.h>
#include "globals.hpp"

bool b_Shutdown = false;
HWND h_Game;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace draw
{
    HWND h_hWnd;
    WNDCLASSEX wc;

    ID3D11Device* g_pd3dDevice = NULL;
    ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
    IDXGISwapChain* g_pSwapChain = NULL;
    ID3D11RenderTargetView* g_mainRenderTargetView = NULL;


    auto CleanupRenderTarget() -> void
    {
        if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
    }

    auto CleanupDeviceD3D() -> void
    {
        CleanupRenderTarget();
        if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
        if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
        if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    }

    void CreateRenderTarget()
    {
        ID3D11Texture2D* pBackBuffer;
        g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }

    bool CreateDeviceD3D()
    {
        /* setup swap chain */
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = h_hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
        if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
            return false;

        CreateRenderTarget();
        return true;
    }

    void Shutdown()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        CleanupDeviceD3D();
        DestroyWindow(h_hWnd);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        std::exit(0);
    }

    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application Render
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        default: break;
        }

        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    /**
     * initializes the renderer
     */
    bool Initialize()
    {
        bool is_menu = false;
        WINDOWPLACEMENT g_wpPrev;
        DWORD dwStyle = GetWindowLong(h_Game, GWL_STYLE);
        MONITORINFO mi = { sizeof(mi) };
        if (GetWindowPlacement(h_Game, &g_wpPrev) &&
            GetMonitorInfo(MonitorFromWindow(h_Game,
                MONITOR_DEFAULTTOPRIMARY), &mi)) {
            SetWindowLong(h_Game, GWL_STYLE,
                dwStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(h_Game, HWND_TOP,
                mi.rcMonitor.left, mi.rcMonitor.top,
                mi.rcMonitor.right - mi.rcMonitor.left,
                mi.rcMonitor.bottom - mi.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }

        // Create application window
        wc = { sizeof(WNDCLASSEX), ACS_TRANSPARENT, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, ("Windows Task Assistant"), NULL };
        RegisterClassEx(&wc);
        h_hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LAYERED, wc.lpszClassName, (""), WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, wc.hInstance, NULL);

        MARGINS margins = { -1 };
        DwmExtendFrameIntoClientArea(h_hWnd, &margins);

        SetLayeredWindowAttributes(h_hWnd, 0, 1.0f, LWA_ALPHA);
        SetLayeredWindowAttributes(h_hWnd, 0, RGB(0, 0, 0), LWA_COLORKEY);

        // Initialize Direct3D
        if (!CreateDeviceD3D())
        {
            CleanupDeviceD3D();
            UnregisterClass(wc.lpszClassName, wc.hInstance);
            return 1;
        }

        ShowWindow(h_hWnd, SW_SHOWDEFAULT);
        UpdateWindow(h_hWnd);

        SetForegroundWindow(h_hWnd);

        //IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplWin32_Init(h_hWnd);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

        MSG msg;
        ZeroMemory(&msg, sizeof(msg));

        while (msg.message != WM_QUIT)
        {
            if (b_Shutdown)
                break;

            if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                continue;
            }

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            if (is_menu)
            {
                ImGui::Begin("Invaded", 0, 0);
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.612, 0.f, 0.f, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.58, 0.f, 0.f, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.612, 0.f, 0.f, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.075, 0.078, 0.094, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(118.f, 0.f, 0.f, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.612, 0.f, 0.f, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.612, 0.f, 0.f, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.687, 0.f, 0.f, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.612, 0.f, 0.f, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.075, 0.078, 0.094, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.075, 0.078, 0.094, 1.f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.075, 0.078, 0.094, 1.f));;
                    ImGui::Text("In Raid"); ImGui::SameLine(); ImGui::ToggleButton("#raidtoggle", &settings::is_in_raid);
                    if (settings::is_in_raid)
                    {
                        if (ImGui::TreeNode("Exploits"))
                        {
                            ImGui::Text("Infinite Stamina"); ImGui::SameLine(); ImGui::ToggleButton("#staminatoggle", &settings::is_infinite_stamina);
                            ImGui::Text("No Visor"); ImGui::SameLine(); ImGui::ToggleButton("#visortoggle", &settings::is_no_visor);
                            ImGui::Text("Thermal Vision"); ImGui::SameLine(); ImGui::ToggleButton("#thermaltoggle", &settings::is_thermal_vision);
                            ImGui::TreePop();
                        }
                        if (ImGui::TreeNode("Weapon Mods"))
                        {
                            ImGui::Text("No Recoil"); ImGui::SameLine(); ImGui::ToggleButton("#recoiltoggle", &settings::is_no_recoil);
                            ImGui::Text("No Spread"); ImGui::SameLine(); ImGui::ToggleButton("#spreadtoggle", &settings::is_no_spread);
                            ImGui::Text("Instant Aim"); ImGui::SameLine(); ImGui::ToggleButton("#aimtoggle", &settings::is_instant_aim);
                            ImGui::TreePop();
                        }
                        if (ImGui::TreeNode("Aim Settings"))
                        {
                            ImGui::Text("Aim"); ImGui::SameLine(); ImGui::ToggleButton("#aimtoggle", &settings::is_aimbot);
                            if (settings::is_aimbot)
                            {
                                ImGui::Text("Show FOV"); ImGui::SameLine(); ImGui::ToggleButton("#fovtoggle", &settings::aim::enable_fov_circle);
                                ImGui::Text("Aim FOV");
                                ImGui::SliderFloat("#fovslider", &settings::aim::aim_fov, 10, 500, "%.3f", 0);
                            }
                            ImGui::TreePop();
                        }
                        if (ImGui::TreeNode("ESP Settings"))
                        {
                            ImGui::Text("ESP"); ImGui::SameLine(); ImGui::ToggleButton("#esptoggle", &settings::is_esp);
                            if (settings::is_esp)
                            {
                                ImGui::Text("Show Role"); ImGui::SameLine(); ImGui::ToggleButton("#roletoggle", &settings::esp::show_role);
                            }
                            ImGui::TreePop();
                        }
                    }
                }
                ImGui::PopStyleColor(12);
                ImGui::End();
            }

            ImDrawList* draw_list = ImGui::GetForegroundDrawList();

            for (auto& info : vars::drawing_list)
            {
                float height = info.Base_Position.y - info.Head_Position.y;
                float width = height / 2.f;

                float x = info.Base_Position.x - (width / 2.f);
                float y = info.Head_Position.y;

                draw_list->AddRect(ImVec2(x, y), ImVec2(x + width, y + height), ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), 0.f, 0, 1.5);

                if (settings::esp::show_role)
                {
                    ImVec2 text_size = ImGui::CalcTextSize2(info.p_info.type.c_str(), 12.f);
                    draw_list->AddText(ImVec2(info.Base_Position.x - text_size.x / 2, y + height + text_size.y), ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), info.p_info.type.c_str(), NULL, NULL, 12.f);
                }
               
            }

            if (settings::aim::enable_fov_circle)
                draw_list->AddCircle(ImVec2(ScreenCenterX, ScreenCenterY), settings::aim::aim_fov, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), 0, 1.f);

            ImGui::EndFrame();

            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);

            ImVec4 clear_color = ImVec4(0, 0, 0, 0);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);

            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            g_pSwapChain->Present(0, 0);

            if (GetAsyncKeyState(VK_INSERT) != 0)
                is_menu = !is_menu;

            long style = GetWindowLongPtr(h_hWnd, GWL_EXSTYLE);


            if (is_menu)
            {
                style &= ~WS_EX_LAYERED;
                SetWindowLongPtr(h_hWnd, GWL_EXSTYLE, style);
                Sleep(10);
            }
            else
            {
                style |= WS_EX_LAYERED;
                SetWindowLongPtr(h_hWnd, GWL_EXSTYLE, style);
            }

        }
        Shutdown();
        return 0;
    }
}