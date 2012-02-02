#ifndef __TOOLS_GUI_EVENTS_MOUSEMOVE_HPP__
#define __TOOLS_GUI_EVENTS_MOUSEMOVE_HPP__

#include "tools/gui/Event.hpp"

namespace Tools { namespace Gui { namespace Events {

    struct MouseMove : public Tools::Gui::Event
    {
        int x, y;
        int dx, dy;

        MouseMove(int x, int y) :
            x(x), y(y), dx(0), dy(0)
        {
        }

        MouseMove(int x, int y, int dx, int dy) :
            x(x), y(y), dx(dx), dy(dy)
        {
        }
    };

}}}

#endif
