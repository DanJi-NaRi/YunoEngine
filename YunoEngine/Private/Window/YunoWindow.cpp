#include "pch.h"

//유노
#include "YunoWindow.h"

YunoWindow::YunoWindow() = default;
YunoWindow::~YunoWindow() = default;

bool YunoWindow::Create(const wchar_t* title, uint32_t width, uint32_t height)
{
    HINSTANCE hInst = GetModuleHandle(nullptr);

    const wchar_t* kClassName = L"YunoWin32WindowClass";

    WNDCLASSEX wc{};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = YunoWindow::WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = kClassName;

    RegisterClassEx(&wc);

    RECT rc{ 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    m_hWnd = CreateWindowEx(
        0,
        kClassName,
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        hInst,
        this 
    );


    if (!m_hWnd)
        return false;

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    OnResizeFromClientRect();
    return true;
}

void YunoWindow::PollEvents()
{
    MSG msg{};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)     // 종료만 여기서 처리 나머지 메시지는 프로시저한테 넘김
        {
            m_shouldClose = true;
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void YunoWindow::OnResizeFromClientRect()
{
    RECT rc{};
    GetClientRect(m_hWnd, &rc);
    m_clientWidth = (uint32_t)(rc.right - rc.left);
    m_clientHeight = (uint32_t)(rc.bottom - rc.top);
}

bool YunoWindow::ConsumeResize(uint32_t& outW, uint32_t& outH)
{
    if (!m_resized)
        return false;

    m_resized = false;      // 더티플래그
    outW = m_pendingW;
    outH = m_pendingH;
    return true;
}

LRESULT CALLBACK YunoWindow::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    YunoWindow* window = nullptr;

    if (msg == WM_NCCREATE)
    {
        auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = static_cast<YunoWindow*>(cs->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
    }
    else
    {
        window = reinterpret_cast<YunoWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    switch (msg)
    {
    case WM_CLOSE:
        if (window) window->m_shouldClose = true;
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            SendMessage(hWnd, WM_DESTROY, 0, 0);
            break;
        }
        return 0;

    case WM_SIZE:
        if (window)
        {
            const uint32_t w = static_cast<uint32_t>(LOWORD(lParam));
            const uint32_t h = static_cast<uint32_t>(HIWORD(lParam));

            window->m_clientWidth = w;
            window->m_clientHeight = h;

            window->m_pendingW = w;
            window->m_pendingH = h;
            window->m_resized = true;   //더티 플래그
        }
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
