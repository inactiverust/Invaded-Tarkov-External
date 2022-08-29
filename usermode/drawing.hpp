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
bool is_menu = false;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace draw
{
    HWND h_hWnd;
    WNDCLASSEX wc;

    ID3D11Device* g_pd3dDevice = NULL;
    ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
    IDXGISwapChain* g_pSwapChain = NULL;
    ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

    static const char* options[]{ "Head", "Chest", "Leg" };
    int selected;

    void UpdateWND()
    {
        while (true)
        {
            if (GetAsyncKeyState(VK_INSERT) != 0)
                is_menu = !is_menu;

            long style = GetWindowLongPtr(h_hWnd, GWL_EXSTYLE);


            if (is_menu)
            {
                style &= ~WS_EX_LAYERED;
                SetWindowLongPtr(h_hWnd, GWL_EXSTYLE, style);
            }
            else
            {
                style |= WS_EX_LAYERED;
                SetWindowLongPtr(h_hWnd, GWL_EXSTYLE, style);
            }
            Sleep(100);
        }
    }

    void draw_box(ImVec2 p1, ImVec2 p2, ImDrawList* draw_list)
    {
        draw_list->AddRect(p1, p2, ImGui::ColorConvertFloat4ToU32(ImVec4(settings::esp::box_color.x, settings::esp::box_color.y, settings::esp::box_color.z, 1)), 0.f, 0, 1.5);
    }

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
        sd.BufferDesc.RefreshRate.Numerator = 300;
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
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)UpdateWND, 0, 0, 0);
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
                    ImGui::Text("Is In Raid"); ImGui::SameLine(); ImGui::ToggleButton("#raidtoggle", &settings::is_in_raid);
                    if (settings::is_in_raid)
                    {
                        if (ImGui::TreeNode("Exploits"))
                        {
                            ImGui::Text("Infinite Stamina"); ImGui::SameLine(); ImGui::ToggleButton("#staminatoggle", &settings::is_infinite_stamina);
                            ImGui::Text("Chams"); ImGui::SameLine(); ImGui::ToggleButton("#chamtoggle", &settings::is_chams);
                            ImGui::Text("Loot Through Walls"); ImGui::SameLine(); ImGui::ToggleButton("#loottoggle", &settings::is_loot_thru_walls);
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
                                ImGui::Text("Show Aim Line"); ImGui::SameLine(); ImGui::ToggleButton("#aimlinetoggle", &settings::esp::show_aim_line);
                                ImGui::Text("Aim FOV");
                                ImGui::SliderFloat("#fovslider", &settings::aim::aim_fov, 10, 500, "%.3f", 0);
                                ImGui::Text("Max Aim Distance");
                                ImGui::SliderFloat("#adistanceslider", &settings::aim::max_distance, 10, 400, "%.3f", 0);
                                ImGui::Text("Aim Bone"); ImGui::SameLine(); ImGui::Combo(" ", &selected, options, IM_ARRAYSIZE(options));
                                
                                switch (selected)
                                {
                                case 0:
                                    settings::aim::aim_bone = Bone::bones::HumanHead;
                                    break;
                                case 1:
                                    settings::aim::aim_bone = Bone::bones::HumanSpine2;
                                    break;
                                case 2:
                                    settings::aim::aim_bone = Bone::bones::HumanLCalf;
                                    break;
                                }

                            }
                            ImGui::TreePop();
                        }
                        if (ImGui::TreeNode("ESP Settings"))
                        {
                            if (ImGui::TreeNode("Player ESP"))
                            {
                                ImGui::Text("ESP"); ImGui::SameLine(); ImGui::ToggleButton("#esptoggle", &settings::is_esp);
                                if (settings::is_esp)
                                {
                                    ImGui::Text("Role"); ImGui::SameLine(); ImGui::ToggleButton("#roletoggle", &settings::esp::show_role);
                                    ImGui::Text("Distance"); ImGui::SameLine(); ImGui::ToggleButton("#distancetoggle", &settings::esp::show_distance);
                                    ImGui::Text("Health Bar"); ImGui::SameLine(); ImGui::ToggleButton("#healthtoggle", &settings::esp::show_health);
                                    ImGui::Text("Max Player Distance");
                                    ImGui::SliderFloat("#pdistanceslider", &settings::esp::max_distance, 10, 400, "%.3f", 0);
                                }
                                ImGui::TreePop();
                            }
                            if (ImGui::TreeNode("Item ESP"))
                            {
                                ImGui::Text("Loot ESP"); ImGui::SameLine(); ImGui::ToggleButton("#loottoggle", &settings::esp::is_loot_esp);
                                if (settings::esp::is_loot_esp)
                                {
                                    ImGui::Text("Max Loot Distance");
                                    ImGui::SliderFloat("#ldistanceslider", &settings::esp::loot_esp::max_distance, 10, 400, "%.3f", 0);
                                    ImGui::Text("Min Loot Price");
                                    ImGui::SliderFloat("#priceslider", &settings::esp::loot_esp::min_price, 1000, 500000, "%.3f", 0);
                                }
                                ImGui::TreePop();
                            }
                            ImGui::Text("Extracts"); ImGui::SameLine(); ImGui::ToggleButton("#extracttoggle", &settings::esp::show_extracts);
                            ImGui::TreePop();
                        }
                        if (ImGui::TreeNode("Color Settings"))
                        {
                            float col1[3] = { settings::esp::box_color.x, settings::esp::box_color.y, settings::esp::box_color.z };
                            ImGui::ColorEdit3("Box Color", col1, 0);
                            settings::esp::box_color.x = col1[0]; settings::esp::box_color.y = col1[1]; settings::esp::box_color.z = col1[2];
                            float col2[3] = { settings::esp::loot_esp::loot_color.x, settings::esp::loot_esp::loot_color.y, settings::esp::loot_esp::loot_color.z };
                            ImGui::ColorEdit3("Loot ESP Color", col2, 0);
                            settings::esp::loot_esp::loot_color.x = col2[0]; settings::esp::loot_esp::loot_color.y = col2[1]; settings::esp::loot_esp::loot_color.z = col2[2];
                            float col3[3] = { settings::aim::aim_circle_color.x, settings::aim::aim_circle_color.y, settings::aim::aim_circle_color.z };
                            ImGui::ColorEdit3("FOV Color", col3, 0);
                            settings::aim::aim_circle_color.x = col3[0]; settings::aim::aim_circle_color.y = col3[1]; settings::aim::aim_circle_color.z = col3[2];
                            ImGui::TreePop();
                        }
                    }
                }
                ImGui::PopStyleColor(12);
                ImGui::End();
            }

            ImDrawList* draw_list = ImGui::GetForegroundDrawList();

            if(settings::esp::is_loot_esp)
            {
                for (auto& loot : loot_list.world_loot_list)
                {
                    std::string line1 = loot.short_name + " [" + std::to_string((int)loot.distance) + "]";
                    ImVec2 text_size = ImGui::CalcTextSize2(line1.c_str(), 12.f);
                    draw_list->AddText(ImVec2(loot.location.x - text_size.x / 2, loot.location.y + text_size.y / 2), ImGui::ColorConvertFloat4ToU32(ImVec4(settings::esp::loot_esp::loot_color.x, settings::esp::loot_esp::loot_color.y, settings::esp::loot_esp::loot_color.z, 1)), line1.c_str(), NULL, NULL, 12.f);
                    if(loot.short_name != "Corpse")
                        draw_list->AddText(ImVec2(loot.location.x - text_size.x / 2, loot.location.y + (text_size.y / 2) + 10), ImGui::ColorConvertFloat4ToU32(ImVec4(settings::esp::loot_esp::loot_color.x, settings::esp::loot_esp::loot_color.y, settings::esp::loot_esp::loot_color.z, 1)), std::to_string(loot.price).c_str(), NULL, NULL, 12.f);
   
                }
            }
            if (settings::esp::show_extracts)
            {
                for (auto& temp : exfil_list.extract_info_list)
                {
                    draw_list->AddText(ImVec2(temp.screen_pos.x, temp.screen_pos.y), ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), (temp.name + "[" + std::to_string((int)temp.distance) + "m]").c_str(), NULL, NULL, 12.f);
                }
            }
            if (settings::is_esp)
            {
                for (auto& info : vars::drawing_list)
                {
                    float height = info.Base_Position.y - info.Head_Position.y;
                    float width = height / 2.f;

                    float x = info.Base_Position.x - (width / 2.f);
                    float y = info.Head_Position.y;

                    draw_box(ImVec2(x, y), ImVec2(x + width, y + height), draw_list);

                    if (settings::is_aimbot && vars::aim_player && settings::esp::show_aim_line)
                    {
                        draw_list->AddLine(ImVec2(ScreenCenterX, ScreenCenterY), ImVec2(vars::target_info.PlayerPos.x, vars::target_info.PlayerPos.y), ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), 1.f);
                    }

                    if (settings::esp::show_health)
                    {
                        draw_list->AddLine(ImVec2(x + width * 1.1, y + height), ImVec2(x + width * 1.1, y + (height - height * info.p_info.health)), IM_COL32(0, 255, 0, 255), 1.5);
                    }
                    if (settings::esp::show_role)
                    {
                        std::string name = info.p_info.type;
                        if (settings::esp::show_distance)
                        {
                            name.append(" [" + std::to_string(info.p_info.distance) + "m]");
                        }
                        ImVec2 text_size = ImGui::CalcTextSize2(name.c_str(), 12.f);
                        draw_list->AddText(ImVec2(info.Base_Position.x - text_size.x / 2, y + height + text_size.y / 2), ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), name.c_str(), NULL, NULL, 12.f);
                    }
                    else if (settings::esp::show_distance)
                    {
                        std::string distance = "[" + std::to_string(info.p_info.distance) + "]";
                        ImVec2 text_size = ImGui::CalcTextSize2(distance.c_str(), 12.f);
                        draw_list->AddText(ImVec2(info.Base_Position.x - text_size.x / 2, y + height + text_size.y / 2), ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1)), distance.c_str(), NULL, NULL, 12.f);
                    }
                }
            }

            if (settings::aim::enable_fov_circle)
                draw_list->AddCircle(ImVec2(ScreenCenterX, ScreenCenterY), settings::aim::aim_fov, ImGui::ColorConvertFloat4ToU32(ImVec4(settings::aim::aim_circle_color.x, settings::aim::aim_circle_color.y, settings::aim::aim_circle_color.z, 1)), 0, 1.f);

            ImGui::EndFrame();

            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);

            ImVec4 clear_color = ImVec4(0, 0, 0, 0);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);

            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            g_pSwapChain->Present(0, 0);
        }
        Shutdown();
        return 0;
    }
}