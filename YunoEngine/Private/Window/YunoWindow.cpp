#include "pch.h"

//유노
#include "YunoWindow.h"
#include "YunoEngine.h"
#include "ImGuiManager.h"
#include "IInput.h"
#include "InputEvent.h"

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
    wc.hCursor = nullptr;
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

void YunoWindow::SetClientSize(uint32_t width, uint32_t height)
{
    RECT rc{ 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    SetWindowPos(
        m_hWnd,
        nullptr,
        0, 0,
        rc.right - rc.left,
        rc.bottom - rc.top,
        SWP_NOMOVE | SWP_NOZORDER
    );
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


#ifdef _DEBUG
    if (ImGuiManager::ImGuiOwnWndProc(hWnd, msg, wParam, lParam))
        return true;
#endif
    

    //마우스 트랙킹 활성화.
    if (IInput* input = YunoEngine::GetInput()) input->MouseTrack(hWnd, true);


    switch (msg)
    {
    case WM_CLOSE:
    {
        if (window) window->m_shouldClose = true;
        DestroyWindow(hWnd);
        return 0;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_KEYDOWN:
    {

        if (IInput* input = YunoEngine::GetInput())
        {
            InputEvent evt{};
            evt.type = InputEventType::KeyDown;
            evt.key = static_cast<uint32_t>(wParam); // 키 코드(어떤 키인지)
            input->PushEvent(evt);
        }

        if (wParam == VK_ESCAPE)
        {
            //SendMessage(hWnd, WM_DESTROY, 0, 0);
        }

        return 0;
    }
    case WM_MOUSEHOVER: // 마우스 호버링
    {
        if (IInput* input = YunoEngine::GetInput())
        {
            InputEvent evt{};
            evt.type = InputEventType::MouseHover;
            evt.x = static_cast<float>(GET_X_LPARAM(lParam));
            evt.y = static_cast<float>(GET_Y_LPARAM(lParam));
            input->PushEvent(evt);
        }
        return 0;
    }
    return 0;
    case WM_MOUSEMOVE:
    {
        if (IInput* input = YunoEngine::GetInput())
        {
            InputEvent evt{};
            evt.type = InputEventType::MouseMove;
            // [TODO] 클라이언트 바깥으로 포인터 나갈 때 디버깅
            evt.x = static_cast<float>(GET_X_LPARAM(lParam));
            evt.y = static_cast<float>(GET_Y_LPARAM(lParam));
            input->PushEvent(evt);
        }
        return 0;
    }
    case WM_KEYUP:
    {
        if (IInput* input = YunoEngine::GetInput())
        {
            InputEvent evt{};
            evt.type = InputEventType::KeyUp;
            evt.key = static_cast<uint32_t>(wParam);
            input->PushEvent(evt);
        }
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        if (IInput* input = YunoEngine::GetInput())
        {
            InputEvent evt{};
            evt.type = InputEventType::MouseButtonDown;
            evt.key = 0;
            evt.x = (float)GET_X_LPARAM(lParam);
            evt.y = (float)GET_Y_LPARAM(lParam);
            input->PushEvent(evt);
        }

        window->m_isClicking = true;
        SetCursor(window->m_clickCursor);

        break;
    }
    case WM_LBUTTONUP:
    {
        if (IInput* input = YunoEngine::GetInput())
        {
            InputEvent evt{};
            evt.type = (msg == WM_LBUTTONDOWN) ? InputEventType::MouseButtonDown
                : InputEventType::MouseButtonUp;
            evt.key = 0; // LMB
            evt.x = static_cast<float>(GET_X_LPARAM(lParam));
            evt.y = static_cast<float>(GET_Y_LPARAM(lParam));
            input->PushEvent(evt);
        }
        window->m_isClicking = false;
        SetCursor(window->m_normalCursor);
        return 0;
    }
    //case WM_RBUTTONDOWN:
    //case WM_RBUTTONUP:
    //{
    //    if (IInput* input = YunoEngine::GetInput())
    //    {
    //        InputEvent evt{};
    //        evt.type = (msg == WM_RBUTTONDOWN) ? InputEventType::MouseButtonDown
    //            : InputEventType::MouseButtonUp;
    //        evt.key = 1; // RMB
    //        evt.x = static_cast<float>(GET_X_LPARAM(lParam));
    //        evt.y = static_cast<float>(GET_Y_LPARAM(lParam));
    //        input->PushEvent(evt);
    //    }
    //    return 0;
    //}
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    {
        if (IInput* input = YunoEngine::GetInput())
        {
            InputEvent evt{};
            evt.type = (msg == WM_MBUTTONDOWN) ? InputEventType::MouseButtonDown
                : InputEventType::MouseButtonUp;
            evt.key = 2; // MMB
            input->PushEvent(evt);
        }
        return 0;
    }
    case WM_MOUSEWHEEL:
    {
        if (IInput* input = YunoEngine::GetInput())
        {
            InputEvent evt{};
            evt.type = InputEventType::MouseWheel;
            evt.delta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
            input->PushEvent(evt);
        }
        return 0;
    }
    case WM_MOUSELEAVE:
    {
        if (IInput* input = YunoEngine::GetInput())
        {
            InputEvent evt{};
            evt.type = InputEventType::MouseLeave;
            input->PushEvent(evt);
        }
        return 0;
    }
    case WM_SETCURSOR:
    {
        if (LOWORD(lParam) == HTCLIENT && window)
        {
            if (window->m_isClicking)
                SetCursor(window->m_clickCursor);
            else
                SetCursor(window->m_normalCursor);

            return TRUE;
        }
        break;
    }
    case WM_SIZE:
    {
        if (window)
        {
            const uint32_t w = static_cast<uint32_t>(LOWORD(lParam));
            const uint32_t h = static_cast<uint32_t>(HIWORD(lParam));


            if (wParam == SIZE_MINIMIZED || w == 0 || h == 0)
            {
                window->m_minimized = true;

                window->m_clientWidth = w;
                window->m_clientHeight = h;

                window->m_resized = false;
                return 0;
            }

            window->m_minimized = false;

            window->m_clientWidth = w;
            window->m_clientHeight = h;

            window->m_pendingW = w;
            window->m_pendingH = h;
            window->m_resized = true;
        }
        return 0;
    }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void YunoWindow::InitializeCursor(const wchar_t* normalPath, const wchar_t* clickPath)
{
    m_normalCursor = LoadCursorFromFile(normalPath);
    m_clickCursor = LoadCursorFromFile(clickPath);

    SetClassLongPtr(m_hWnd, GCLP_HCURSOR,
        (LONG_PTR)m_normalCursor);

    SetCursor(m_normalCursor);
}

void YunoWindow::SetClickState(bool isClicking)
{
    m_isClicking = isClicking;
}
