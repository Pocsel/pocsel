#ifndef __TOOLS_GUI_EVENTMANAGERSDL_HPP__
#define __TOOLS_GUI_EVENTMANAGERSDL_HPP__

#include <string>
#include <list>

#include "events/Key.hpp"
#include "events/MouseButton.hpp"

namespace Tools { namespace Gui {

    class Window;
    struct Event;

    class EventManagerImplem
    {
    private:
        Window& _window;
        std::list<Events::Key> _keysHeld;
        std::list<Events::MouseButton> _buttonsHeld;

    public:
        EventManagerImplem(Window& window);
        unsigned int Poll();
        void Stop();

    private:
        void _Fire(std::string const& id, Event const& ev);
    };

}}

#endif
