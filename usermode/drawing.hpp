#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d9.h>
#include <random>
#include "globals.hpp"

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

std::string random_string()
{
	std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

	std::random_device rd;
	std::mt19937 generator(rd());

	std::shuffle(str.begin(), str.end(), generator);

	return str.substr(0, 10);    // assumes 32 < number of characters in str         
}

namespace menu
{
	/*
	void draw_overlay()
	{
		std::string rand = random_string();
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, rand.c_str(), NULL };
		::RegisterClassEx(&wc);
		HWND hwnd = ::CreateWindow(wc.lpszClassName, rand.c_str(), WS_POPUP, 5, 5, 10, 10, NULL, NULL, wc.hInstance, NULL);
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		if (!CreateDeviceD3D(hwnd))
		{
			CleanupDeviceD3D();
			::UnregisterClass(wc.lpszClassName, wc.hInstance);
			return;
		}
		::ShowWindow(hwnd, SW_SHOWDEFAULT);
		::UpdateWindow(hwnd);
		bool done = false;
		while (!done)
		{
			::MoveWindow(hwnd, 0, 0, ScreenCenterX, ScreenCenterY, false);
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					done = true;
			}
			if (done)
				break;


			// Rendering
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
			D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
			if (g_pd3dDevice->BeginScene() >= 0)
			{
				g_pd3dDevice->EndScene();
			}
			HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

			// Handle loss of D3D9 device
			if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				ResetDevice();
		}

		CleanupDeviceD3D();
		::DestroyWindow(hwnd);
		::UnregisterClass(wc.lpszClassName, wc.hInstance);

	}
	*/
	void render()
	{
		std::string rand = random_string();
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, rand.c_str(), NULL };
		::RegisterClassEx(&wc);
		HWND hwnd = ::CreateWindow(wc.lpszClassName, rand.c_str(), WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, 50, 50, 450, 600, NULL, NULL, wc.hInstance, NULL);
		if (!CreateDeviceD3D(hwnd))
		{
			CleanupDeviceD3D();
			::UnregisterClass(wc.lpszClassName, wc.hInstance);
			return;
		}
		::ShowWindow(hwnd, SW_SHOWDEFAULT);
		::UpdateWindow(hwnd);
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX9_Init(g_pd3dDevice);
		int selectedBone = 0;
		bool done = false;
		while (!done)
		{
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					done = true;
			}
			if (done)
				break;

			// Start the Dear ImGui frame
			static const char* options[]{ "Head", "Chest", "Leg" };
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(450, 600));
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

			static const auto dwFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar;
			if (!settings::connected)
			{
				ImGui::Begin("Invaded", 0, dwFlags);
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.612, 0.f, 0.f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.58, 0.f, 0.f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.612, 0.f, 0.f, 1.f));
					ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.075, 0.078, 0.094, 1.f));
					if (ImGui::Button("Open Game and Then Click Me"))
						settings::connected = true;
					ImGui::PopStyleColor(4);
				}
				ImGui::End();
			}
			else
			{
				ImGui::Begin("Invaded", 0, dwFlags);
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
							ImGui::TreePop();
						}
					}
				}
				ImGui::PopStyleColor(12);
				ImGui::End();
			}
			// Rendering
			ImGui::EndFrame();
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
			D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
			if (g_pd3dDevice->BeginScene() >= 0)
			{
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				g_pd3dDevice->EndScene();
			}
			HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

			// Handle loss of D3D9 device
			if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				ResetDevice();
		}

		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		::DestroyWindow(hwnd);
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;
	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
}