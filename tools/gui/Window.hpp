#ifndef __TOOLS_GUI_WINDOW_HPP__
#define __TOOLS_GUI_WINDOW_HPP__

#include <string>

#include "tools/IRenderer.hpp"

#include "EventManager.hpp"
#include "Viewport.hpp"

namespace Tools { namespace Gui {

    class WindowImplem;

    class Window : private boost::noncopyable
    {
    private:
        Viewport            _viewport;
        WindowImplem*       _implem;
        IRenderer&          _renderer;
        bool                _hasExpose;
        bool                _cursorAutoCenter;
        bool                _justAutoCentered;
        EventManager*       _eventManager;

    public:
        Window(std::string const& title = "",
               size_t width = 640,
               size_t height = 480);
        virtual ~Window();

    public: // getter / setter
        Vector2<Uint32> const& GetSize() { return this->_viewport.size(); }
        void SetSize(Vector2<Uint32> const& size) { this->_viewport.size(size); }
        void SetSize(Uint32 w, Uint32 h) { this->_viewport.size(w, h); }
        Viewport& GetViewport() { return this->_viewport; }
        IRenderer& GetRenderer() { return this->_renderer; }
        EventManager& GetEventManager() { return *this->_eventManager; }
        WindowImplem& GetImplem() { return *this->_implem; }
        virtual void Render() {};

    protected:
        void _Render();
        void _OnMouseMove(Events::MouseMove const& evt);
        void _OnExpose(Events::Expose const& evt);
        void _OnKey(Events::Key const& evt);
        void _OnMouseButton(Events::MouseButton const& evt);
        void _OnResize(Events::Resize const& evt);
        void _OnQuit();
        void _OnCursorShow(Events::Boolean const& evt);
        void _OnCursorAutoCenter(Events::Boolean const& evt);
    };

}}

#endif
