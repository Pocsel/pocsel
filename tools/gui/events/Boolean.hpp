#ifndef __TOOLS_GUI_EVENTS_BOOLEAN_HPP__
#define __TOOLS_GUI_EVENTS_BOOLEAN_HPP__

#include "tools/gui/Event.hpp"

namespace Tools { namespace Gui { namespace Events {

    struct Boolean : public Tools::Gui::Event
    {
        union
        {
            bool b;
            bool on;
            bool value;
        };

        Boolean(bool const& b) : b(b) {}
    };

}}}

#endif
