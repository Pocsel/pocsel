#ifndef __TOOLS_GUI_WINDOWWINDOWS_HPP__
#define __TOOLS_GUI_WINDOWWINDOWS_HPP__

#ifdef _WIN32

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Vector2.hpp"

namespace Tools { namespace Gui {

    class WindowImplem
    {
    public:
        typedef std::function<long(WindowImplem&, UINT, WPARAM, LPARAM)> Callback;

    private:
        HWND _windowHandle;
        HDC _hdc;

        HGLRC _glContext;
        IRenderer* _renderer;

        Callback* _winProcCallback;

        Vector2<int>                _pos;
        Vector2<std::size_t>             _size;

        static std::map<HWND, WindowImplem*> _hwndToWindowImplem;

    public:
        WindowImplem(std::string const& title, size_t width, size_t height);
        virtual ~WindowImplem();
        void Render();
        void SwapBuffers();

        IRenderer& GetRenderer() { return *this->_renderer; }
        ::HWND GetHwnd() { return this->_windowHandle; }
        ::HDC GetHdc() { return this->_hdc; }
        size_t GetWidth() const { return this->_size.w; }
        size_t GetHeight() const { return this->_size.h; }
        Vector2<int> const& GetPos() const { return this->_pos; }
        Vector2<size_t> const& GetSize(bool force_refresh = false);
        void SetWinProcCallback(Callback* callback);

        void Update();
        void WarpCursor(unsigned int x, unsigned int y);
        void CenterCursor();
        void ShowCursor();
        void HideCursor();

    private:
        static LRESULT CALLBACK _WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        void _CreateGLContext();
    };

}}

#endif // _WIN32

#endif
