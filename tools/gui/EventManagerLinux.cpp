#if 0

#include "events/all.hpp"

#include "EventManagerLinux.hpp"
#include "Window.hpp"
#include "WindowLinux.hpp"


using namespace Tools::Gui;

EventManagerImplem::EventManagerImplem(Window& window) :
    _window(window)
{}

EventManagerImplem::~EventManagerImplem()
{}

unsigned int EventManagerImplem::Poll()
{
    unsigned int count = 0;

    XEvent ev;
    ::Display* dpy = this->_window.GetImplem().GetDisplay();
    if (dpy == 0)
        return 0;

    Events::Resize resizeEvent(0, 0);
    while (::XPending(dpy) > 0)
    {
        XNextEvent(dpy, &ev);

        if (ev.type == Expose)
        {
# ifdef Expose
#  undef Expose //Warning, Cannot use Expose from Here
# endif
            auto s = this->_window.GetImplem().GetSize(true);

            this->_Fire(
                "on-expose",
                Events::Expose(s.w, s.h)
            );
            std::cout << "onExpose " << ev.xexpose.width << ' ' << ev.xexpose.height << std::endl;
        }
        else if (ev.type == KeyPress)
        {
            this->_Fire(
                "on-key",
                Events::Key(Events::Key::Press, ev.xkey.keycode, ev.xkey.state)
            );
        }
        else if (ev.type == KeyRelease)
        {
            this->_Fire(
                "on-key",
                Events::Key(Events::Key::Release, ev.xkey.keycode, ev.xkey.state)
            );
        }
        else if (ev.type == ClientMessage) //ev.type == DestroyNotify ||
        {
            this->_Fire("on-quit", Events::Quit());
            return ++count;
        }
        else if (ev.type == ButtonRelease)
        {
            this->_Fire(
                "on-mouse-button",
                Events::MouseButton(
                    Events::MouseButton::Event::Release,
                    ev.xbutton.x, ev.xbutton.y
                )
            );
        }
        else if (ev.type == MotionNotify)
        {
            this->_Fire(
                "on-mouse-move",
                Events::MouseMove(ev.xmotion.x, ev.xmotion.y)
            );
        }
        else if (ev.type == ConfigureNotify)
        {
            XConfigureEvent config_ev = ev.xconfigure;
            resizeEvent.width = config_ev.width;
            resizeEvent.height = config_ev.height;
        }
        count++;
    }

    if (resizeEvent.width != 0 || resizeEvent.height != 0)
        this->_Fire("on-resize", resizeEvent);

    return count;
}

void EventManagerImplem::Stop()
{

}

void EventManagerImplem::_Fire(std::string const& id, Event const& ev)
{
    this->_window.GetEventManager().Fire(id, ev);
}

#endif // __linux__
