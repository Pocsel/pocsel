#if 0

#include <WindowsX.h>

#include "events/all.hpp"

#include "EventManagerWindows.hpp"
#include "Window.hpp"
#include "WindowWindows.hpp"

using namespace Tools::Gui;

EventManagerImplem::EventManagerImplem(Window& window) :
    _window(window)
{
    this->_windowCallback = std::bind(&EventManagerImplem::_WinProc, this,
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    this->_window.GetImplem().SetWinProcCallback(&this->_windowCallback);
}

EventManagerImplem::~EventManagerImplem()
{
    this->_window.GetImplem().SetWinProcCallback(0);
    this->_window.GetImplem().Update();
}

unsigned int EventManagerImplem::Poll()
{
    unsigned int count = 0;

    MSG msg;
    while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);                     // Find out what the message does
        DispatchMessage(&msg);                      // Execute the message
        count++;
    }

    return count;
}

void EventManagerImplem::Stop()
{

}

void EventManagerImplem::_Fire(std::string const& id, Event* ev)
{
    this->_window.GetEventManager().Fire(id, *ev);
}

long EventManagerImplem::_WinProc(WindowImplem& window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    LRESULT ret = 0;

    switch (msg)
    {
    case WM_PAINT:
        BeginPaint(window.GetHwnd(), &ps);
        this->_Fire("on-expose", new Events::Expose(ps.rcPaint.right, ps.rcPaint.bottom));
        EndPaint(window.GetHwnd(), &ps);
        this->_window.GetImplem().GetSize(true);
        break;

    case WM_MOUSEMOVE:
        this->_Fire("on-mouse-move", new Events::MouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        this->_Fire("on-mouse-button", new Events::MouseButton(Events::MouseButton::Press, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
        break;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        this->_Fire("on-mouse-button", new Events::MouseButton(Events::MouseButton::Release, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
        break;

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        this->_Fire("on-key", new Events::Key(Events::Key::Press, wParam, 0));
        break;

    case WM_SYSKEYUP:
    case WM_KEYUP:
        this->_Fire("on-key", new Events::Key(Events::Key::Release, wParam, 0));
        if (wParam == 27)
            this->_Fire("on-quit", new Events::Quit());
        break;

    case WM_SIZE:
        this->_Fire("on-resize", new Events::Resize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
        this->_window.GetImplem().GetSize(true);
        break;

    case WM_CLOSE:
        this->_Fire("on-quit", new Events::Quit());
        break;

    default:
        ret = DefWindowProc(window.GetHwnd(), msg, wParam, lParam);
        break;
    }
    return ret;
}

#endif // _WIN32
