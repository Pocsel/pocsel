#if 0

# include <stdexcept>
# include <map>

# include "tools/renderers/opengl/opengl.hpp"
# include "WindowWindows.hpp"
# include "tools/renderers/GLRenderer.hpp"

using namespace Tools::Gui;

std::string _GetErrorString(int error)
{
    char* lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&lpMsgBuf,
        0,
        NULL);

    std::string ret(lpMsgBuf);
    LocalFree(lpMsgBuf);

    return ret;
}

std::map<HWND, WindowImplem*> WindowImplem::_hwndToWindowImplem = std::map<HWND, WindowImplem*>();

LRESULT CALLBACK WindowImplem::_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto it = WindowImplem::_hwndToWindowImplem.find(hwnd);
    if (it != WindowImplem::_hwndToWindowImplem.end())
    {
        auto self = it->second;
        if (self->_winProcCallback != 0)
            return (*self->_winProcCallback)(*self, msg, wParam, lParam);
    }

    // Very basic event manager
    PAINTSTRUCT ps;
    LRESULT ret = 0;
    switch (msg)
    {
    case WM_PAINT:
        BeginPaint(hwnd, &ps);                          // Init the paint struct
        EndPaint(hwnd, &ps);                            // EndPaint, Clean up
        break;

    default:
        ret = DefWindowProc(hwnd, msg, wParam, lParam);
        break;
    }
    return ret;
}

WindowImplem::WindowImplem(std::string const& title, size_t width, size_t height) :
    _winProcCallback(0)
{
    WNDCLASSEX wc;
    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WindowImplem::_WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    if ((wc.hInstance = ::GetModuleHandle(0)) == 0)
        throw std::runtime_error(ToString("GetModuleHandle error: ") + _GetErrorString(::GetLastError()));
    if ((wc.hIcon = ::LoadIcon(NULL, IDI_APPLICATION)) == 0)
        throw std::runtime_error(ToString("LoadIcon error: ") + _GetErrorString(::GetLastError()));
    if ((wc.hCursor = ::LoadCursor(NULL, IDC_ARROW)) == 0)
        throw std::runtime_error(ToString("LoadCursor error: ") + _GetErrorString(::GetLastError()));
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "TrollWindowClass";
    if ((wc.hIconSm = ::LoadIcon(NULL, IDI_APPLICATION)) == 0)
        throw std::runtime_error(ToString("LoadIcon error: ") + _GetErrorString(::GetLastError()));

    if(::RegisterClassEx(&wc) == 0)
        throw std::runtime_error(ToString("RegisterClassEx error: ") + _GetErrorString(::GetLastError()));

    this->_windowHandle = ::CreateWindowEx(
        0, // Style ++
        "TrollWindowClass", // Class name
        title.c_str(), // title
        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // Style
        CW_USEDEFAULT, // position X
        0, // position Y
        width,
        height,
        0, // Parent
        0, // Menu
        0, // Instance
        0
    );
    if (this->_windowHandle == 0)
        throw std::runtime_error(ToString("CreateWindowEx error: ") + _GetErrorString(::GetLastError()));
    WindowImplem::_hwndToWindowImplem.insert(std::pair<HWND, WindowImplem*>(this->_windowHandle, this));

    if ((this->_hdc = ::GetDC(this->_windowHandle)) == 0)
        throw std::runtime_error(ToString("GetDC error: ") + _GetErrorString(::GetLastError()));

    ::ShowWindow(this->_windowHandle, SW_SHOWNORMAL);

    this->_CreateGLContext();
}

WindowImplem::~WindowImplem()
{
    WindowImplem::_hwndToWindowImplem.erase(this->_windowHandle);

    Delete(this->_renderer);
    ::wglDeleteContext(this->_glContext);
    ::DestroyWindow(this->_windowHandle);
}

Tools::Vector2<size_t> const& WindowImplem::GetSize(bool force_refresh)
{
    if (force_refresh)
    {
        RECT rect;
        ::GetClientRect(this->_windowHandle, &rect);
        this->_size = Vector2<std::size_t>(rect.right - rect.left, rect.bottom - rect.top);
    }
    return this->_size;
}

void WindowImplem::Update()
{
    ::UpdateWindow(this->_windowHandle);
    ::SetFocus(this->_windowHandle);
}

void WindowImplem::WarpCursor(unsigned int x, unsigned int y)
{
    ::SetCursorPos(int(x), int(y));
}

void WindowImplem::CenterCursor()
{
    POINT pt;
    pt.x = 0;
    pt.y = 0;
    ::ClientToScreen(this->_windowHandle, &pt);
    this->WarpCursor(pt.x + this->_size.w / 2, pt.y + this->_size.h / 2);
}

void WindowImplem::ShowCursor()
{
    ::ShowCursor(true);
}

void WindowImplem::HideCursor()
{
    ::ShowCursor(false);
}

void WindowImplem::SwapBuffers()
{
    ::SwapBuffers(this->_hdc);
}

void WindowImplem::Render()
{
    this->SwapBuffers();
}

void WindowImplem::SetWinProcCallback(Callback* callback)
{
    this->_winProcCallback = callback;
}

void WindowImplem::_CreateGLContext()
{
    PIXELFORMATDESCRIPTOR pfd;
    int pixelformat;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;

    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.dwLayerMask = PFD_MAIN_PLANE;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 16;
    pfd.cAccumBits = 0;
    pfd.cStencilBits = 0;

    if ((pixelformat = ::ChoosePixelFormat(this->_hdc, &pfd)) == false)
        throw std::runtime_error(ToString("ChoosePixelFormat error: ") + _GetErrorString(::GetLastError()));

    if (::SetPixelFormat(this->_hdc, pixelformat, &pfd) == false)
        throw std::runtime_error(ToString("SetPixelFormat error: ") + _GetErrorString(::GetLastError()));

    if ((this->_glContext = ::wglCreateContext(this->_hdc)) == 0)
        throw std::runtime_error(ToString("wglCreateContext error: ") + _GetErrorString(::GetLastError()));

    if (!::wglMakeCurrent(this->_hdc, this->_glContext))
        throw std::runtime_error(ToString("wglMakeCurrent error: ") + _GetErrorString(::GetLastError()));

    BOOL (APIENTRY *wglSwapIntervalEXT)(int) = 0;
    const char *extensions = (char*)glGetString(GL_EXTENSIONS);
    if (strstr(extensions, "WGL_EXT_swap_control") != 0)
    {
        wglSwapIntervalEXT = (BOOL(APIENTRY *)(int))wglGetProcAddress("wglSwapIntervalEXT");

        if (wglSwapIntervalEXT)
            wglSwapIntervalEXT(1);
    }

    this->_renderer = new Renderers::GLRenderer();
    this->_renderer->Initialise();
}

#endif
