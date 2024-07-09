#include "pch.h"
#include "Example.h"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance, _In_ LPWSTR cmdLine, _In_ int cmdShow)
{
    const UINT width = 1280, height = 720;
    const UINT canvasWidth = width, canvasHeight = height;

    WNDCLASSEX winClass =
    {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc,
        0,
        0,
        GetModuleHandle(NULL),
        NULL,
        NULL,
        NULL,
        NULL,
        L"Graphics", // lpszClassName, L-string
        NULL
    };

    RegisterClassEx(&winClass);

    // 실제로 그려지는 해상도를 설정하기 위해
    RECT winRect = { 0, 0, width, height };    // set the size, but not the position
    AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

    HWND hwnd = CreateWindow
    (
        winClass.lpszClassName,
        L"Graphics Example",
        WS_OVERLAPPEDWINDOW,
        100,                            // 윈도우 좌측 상단의 x 좌표
        100,                            // 윈도우 좌측 상단의 y 좌표
        winRect.right - winRect.left,   // 윈도우 가로 방향 해상도
        winRect.bottom - winRect.top,   // 윈도우 세로 방향 해상도
        NULL,
        NULL,
        winClass.hInstance,
        NULL
    );

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    auto example = std::make_unique<Example>(hwnd, width, height);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.DisplaySize = ImVec2(width, height);
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplDX11_Init(example->GetDevice(), example->GetDeviceContext());
    ImGui_ImplWin32_Init(hwnd);

    float* color = (float*)&example->GetBackgroundColor();

    // Main message loop
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // Start the Dear ImGui frame
            //ImGui_ImplDX11_NewFrame();//TODO: IMGUI 사용
            //ImGui_ImplWin32_NewFrame();
            //ImGui::NewFrame();
            //ImGui::Begin("Background Color");
            //ImGui::SliderFloat3("RGB(0.0->1.0)", color, 0.0f, 1.0f);
            //ImGui::End();
            //ImGui::Render();

            example->Update();
            example->Render();

            //ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());//TODO: IMGUI 사용

            // switch the back buffer and the front buffer
            example->GetSwapChain()->Present(1, 0);
        }
    }

    // Cleanup
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(hwnd);
    UnregisterClass(winClass.lpszClassName, winClass.hInstance);

    return 0;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Windows procedure
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        // Reset and resize swapchain
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu 
            return 0;
        break;
    case WM_MOUSEMOVE:
        //std::cout << "Mouse " << LOWORD(lParam) << " " << HIWORD(lParam) << std::endl;
        break;
    case WM_LBUTTONUP:
        //std::cout << "WM_LBUTTONUP Left mouse button" << std::endl;
        break;
    case WM_RBUTTONUP:
        //std::cout << "WM_RBUTTONUP Right mouse button" << std::endl;
        break;
    case WM_KEYDOWN:
        //std::cout << "WM_KEYDOWN " << (int)wParam << std::endl;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
