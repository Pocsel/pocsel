#ifndef __TOOLS_GUI_EVENTS_EXPOSE_HPP__
#define __TOOLS_GUI_EVENTS_EXPOSE_HPP__

#include "tools/gui/Event.hpp"
#ifdef Expose
# undef Expose
#endif

namespace Tools { namespace Gui { namespace Events {

    struct Expose : public ::Tools::Gui::Event
    {
        Uint32 width, height;

        Expose(Uint32 w, Uint32 h) : width(w), height(h) {}
    };

}}}

#endif
