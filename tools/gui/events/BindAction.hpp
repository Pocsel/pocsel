#ifndef __TOOLS_GUI_EVENTS_BINDACTION_HPP__
#define __TOOLS_GUI_EVENTS_BINDACTION_HPP__

#include "tools/enum.hpp"
#include "tools/gui/Event.hpp"

namespace Tools { namespace Gui { namespace Events {

    struct BindAction : public Tools::Gui::Event
    {
        ENUM Type
        {
            Released,
            Pressed,
            Held,
        };

        Type type;

        BindAction(Type type) :
            type(type)
        {
        }
    };

}}}

#endif
