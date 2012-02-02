#ifndef __TOOLS_GUI_EVENTS_RESIZE_HPP__
#define __TOOLS_GUI_EVENTS_RESIZE_HPP__

#include "tools/gui/Event.hpp"

namespace Tools { namespace Gui { namespace Events {

    struct Resize : public Tools::Gui::Event
    {
        unsigned int width, height;

        Resize(unsigned int width, unsigned int height) :
            width(width), height(height)
        {}
    };

}}}

#endif
