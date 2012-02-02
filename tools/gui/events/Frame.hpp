#ifndef __TOOLS_GUI_EVENTS_FRAME_HPP__
#define __TOOLS_GUI_EVENTS_FRAME_HPP__

#include "tools/gui/Event.hpp"

namespace Tools { namespace Gui { namespace Events {

    struct Frame : public Tools::Gui::Event
    {
        Uint32 frameTime;

        Frame(Uint32 frameTime) : frameTime(frameTime) {}
    };

}}}

#endif
