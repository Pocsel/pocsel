
#include <iostream>

#include "tools/Delete.hpp"

#include "events/all.hpp"
#include "tools/gui/EventCaster.hpp"

#include "Window.hpp"
#include "WindowImplem.hpp"


#undef Expose
namespace Tools { namespace Gui {

Window::Window(std::string const& title,
               size_t width,
               size_t height) :
    _viewport(),
    _implem(new WindowImplem(title, width, height)),
    _renderer(_implem->GetRenderer()),
    _hasExpose(false),
    _cursorAutoCenter(false),
    _justAutoCentered(false),
    _eventManager(new EventManager(*this))
{
    this->_eventManager->Connect("on-mouse-move", EventCaster<Events::MouseMove, Window>(&Window::_OnMouseMove, this));
    this->_eventManager->Connect("on-mouse-button", EventCaster<Events::MouseButton, Window>(&Window::_OnMouseButton, this));
    this->_eventManager->Connect("on-key", EventCaster<Events::Key, Window>(&Window::_OnKey, this));
    this->_eventManager->Connect("on-resize", EventCaster<Events::Resize, Window>(&Window::_OnResize, this));
    this->_eventManager->Connect("on-expose", EventCaster<Events::Expose, Window>(&Window::_OnExpose, this));
    this->_eventManager->Connect("on-frame", std::bind(&Window::_Render, this));
    this->_eventManager->Connect("on-quit", std::bind(&Window::_OnQuit, this));

    this->_eventManager->Connect("window/cursor-show", EventCaster<Events::Boolean, Window>(&Window::_OnCursorShow, this));
    this->_eventManager->Connect("window/cursor-auto-center", EventCaster<Events::Boolean, Window>(&Window::_OnCursorAutoCenter, this));
}

Window::~Window()
{
    Delete(this->_eventManager);
    Delete(this->_implem);
}

void Window::_Render()
{
    if (!this->_hasExpose)
        return;

    this->_renderer.Clear(ClearFlags::Color | ClearFlags::Depth);

    this->_viewport.Render(this->_renderer);
    this->Render();
    this->_implem->Render();
}


void Window::_OnResize(Events::Resize const& evt)
{
    this->_viewport.OnResize(evt);
    this->_renderer.SetScreenSize(this->GetSize());
    this->_Render();
}


void Window::_OnExpose(Events::Expose const& evt)
{
//    std::cout << "Window::_OnExpose()\n";
    this->_hasExpose = true;
    //this->OnExpose(evt);
    this->SetSize(evt.width, evt.height);
    this->_renderer.SetScreenSize(this->GetSize());
    this->_Render();

    // XXX TODO c'est de la merde mais ça marche:
    this->_OnResize(Tools::Gui::Events::Resize(this->GetSize().w, this->GetSize().h));
}

void Window::_OnKey(Events::Key const& evt)
{
//    std::cout << "Window::_OnKey()\n";
    this->_viewport.OnKey(evt);
}

void Window::_OnMouseMove(Events::MouseMove const& event)
{
//    std::cout << "Window::_OnMouseMove()\n";

    if (this->_justAutoCentered)
    {
        this->_justAutoCentered = false;
        if (this->_cursorAutoCenter)
            return;
    }

    if (this->_cursorAutoCenter)
    {
        Events::MouseMove evt(event.x, event.y);
        evt.dx = (int)(this->_implem->GetWidth() / 2) - event.x;
        evt.dy = (int)(this->_implem->GetHeight() / 2) - event.y;
        this->_viewport.OnMouseMove(evt);
    }
    else
        this->_viewport.OnMouseMove(event);

    if (this->_cursorAutoCenter)
    {
        this->_justAutoCentered = true;
        this->_implem->CenterCursor();
    }
}

void Window::_OnMouseButton(Events::MouseButton const& event)
{
//    std::cout << "Window::_OnMouseButton()\n";
    this->_viewport.OnMouseButton(event);
}

void Window::_OnQuit()
{
    std::cout << "Window::_OnQuit()\n";
    this->_eventManager->Stop();
}

void Window::_OnCursorShow(Events::Boolean const& evt)
{
    if (evt.value)
        this->_implem->ShowCursor();
    else
        this->_implem->HideCursor();
}

void Window::_OnCursorAutoCenter(Events::Boolean const& evt)
{
    this->_cursorAutoCenter = evt.value;
    this->_implem->CenterCursor();
}

}}
