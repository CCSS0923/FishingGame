// Win32 吏꾩엯?먯쑝濡?李??앹꽦, 硫붿떆吏 猷⑦봽, GDI+ 珥덇린?붿? 寃뚯엫 ?ㅽ뻾???대떦?⑸땲??
#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <chrono>
#include "Game.h"
#include "Resource.h"

#pragma comment(lib, "gdiplus.lib")

namespace
{
    Game g_game; // ?꾩뿭 寃뚯엫 ?몄뒪?댁뒪.

    // ?덈룄??硫붿떆吏 泥섎━ ?⑥닔: ?낅젰/由ъ궗?댁쫰/?섏씤?몃? 寃뚯엫?쇰줈 ?꾨떖.
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_SIZE:
            g_game.OnResize(LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_KEYDOWN:
            g_game.OnKeyDown(wParam);
            return 0;
        case WM_KEYUP:
            g_game.OnKeyUp(wParam);
            return 0;
        case WM_LBUTTONDOWN:
            g_game.OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        case WM_LBUTTONUP:
            g_game.OnMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        case WM_MOUSEMOVE:
            g_game.OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            g_game.Render(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }

    // 湲곕낯 ?덈룄?곕? ?앹꽦?쒕떎.
    HWND CreateMainWindow(HINSTANCE hInstance, int width, int height)
    {
        WNDCLASSEX wc{};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
        wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_APP_CURSOR));
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = L"FishingGameWindow";
        wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL_ICON));

        RegisterClassEx(&wc);

        RECT rect{ 0, 0, width, height };
        AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

        HWND hwnd = CreateWindowEx(
            WS_EX_COMPOSITED,
            wc.lpszClassName,
            L"Fishing Game (GDI+)",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr,
            nullptr,
            hInstance,
            nullptr);

        return hwnd;
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    // GDI+ 珥덇린??諛??덈룄??寃뚯엫 猷⑦봽 以鍮?
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) != Gdiplus::Ok)
    {
        return -1;
    }

    const int clientWidth = 1280;
    const int clientHeight = 720;
    HWND hwnd = CreateMainWindow(hInstance, clientWidth, clientHeight);
    if (!hwnd)
    {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    RECT rc{};
    GetClientRect(hwnd, &rc);
    g_game.Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);

    MSG msg{};
    auto prev = std::chrono::steady_clock::now();
    while (true)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                Gdiplus::GdiplusShutdown(gdiplusToken);
                return static_cast<int>(msg.wParam);
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> delta = now - prev;
        prev = now;
        float dt = std::min(delta.count(), 0.05f);

        g_game.Update(dt);
        InvalidateRect(hwnd, nullptr, FALSE);
        Sleep(1);
    }
}
