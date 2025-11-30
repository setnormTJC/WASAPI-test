// WASAPI test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<d3d11.h>
#include<tchar.h> //?

#include"DirectXBoilerplate.h"

#include"ImguiManager.h"

#include"PlayAudioStream.h"
#include"RecordAudioStream.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "AudioManipulation.h"
#include <thread>

#include "MusicNote.h"
#include "MyException.h"



// Global Window Handle
HWND g_hWnd = NULL;



// Forward declaration of ImGui's Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 1. Pass all messages to the ImGui Win32 implementation first.
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			// Handle D3D11 resizing: cleanup, resize swap chain, and recreate render target
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// --- Main Entry Point ---
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR      lpCmdLine,
	int        nCmdShow)
{

	try
	{

		// --- 1. Win32 Window Setup ---
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("WASAPI ImGui Controller"), NULL };
		RegisterClassEx(&wc);

		// Create the application window
		g_hWnd = CreateWindow(wc.lpszClassName, _T("WASAPI Audio Control"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 1800, NULL, NULL, wc.hInstance, NULL);
		if (!g_hWnd) return 1;

		// --- 2. Initialize DirectX 11 ---
		if (!CreateDeviceD3D(g_hWnd))
		{
			CleanupDeviceD3D();
			UnregisterClass(wc.lpszClassName, wc.hInstance);
			return 1;
		}

		// Show the window
		ShowWindow(g_hWnd, SW_SHOWDEFAULT);
		UpdateWindow(g_hWnd);


		// --- 3. Initialize ImGui---
		ImguiManager imgui; // Initialize the ImGui context


		ImGui_ImplWin32_Init(g_hWnd);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

		// Default clear color (dark gray/blue)
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


		//4: Initialize audio manipulation
		AudioManipulation audioManipulation;

		//4b: init music note
		MusicNote musicNote(Duration::Whole, Loudness::Mezzo, "C4");


		// --- 5. Main Loop ---
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			// Process messages waiting in the queue
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				continue;
			}

			// Start the ImGui frame
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// --- Custom GUI Drawing Logic (Next Step Goes Here) ---
			//ImGui::ShowDemoWindow(); // Uncomment to see the demo window

			//std::thread thread(doTheThingOfInterest);

			//thread.join(); 

			audioManipulation.generateTone();

			audioManipulation.generate88Notes(); 

			//audioManipulation.playPureSineNote(); 

			audioManipulation.playSynthesizedNote(); 

			// --- 6. Rendering ---
			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
			g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			g_pSwapChain->Present(1, 0); // Present with vsync

			// --- Main Loop End ---
		}


		// --- 7. Cleanup ---
		// Cleanup ImGui
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		// The ImguiManager destructor handles ImGui::DestroyContext()

		// Cleanup D3D11 and Win32
		CleanupDeviceD3D();
		DestroyWindow(g_hWnd);
		UnregisterClass(wc.lpszClassName, wc.hInstance);
	}

	catch (const MyException& e)
	{
		std::cout << e.whatWentWrong() << "\n";
	}
	return 0;
}
