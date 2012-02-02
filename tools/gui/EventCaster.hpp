#ifndef __TOOLS_GUI_EVENTCASTER_HPP__
#define __TOOLS_GUI_EVENTCASTER_HPP__

#include <functional>
#include "tools/gui/Event.hpp"

namespace Tools { namespace Gui {

    template<typename EventType, typename Class>
    struct EventCaster
    {
    private:
        typedef std::function<void(EventType const&)> CastedCallback;
        CastedCallback cb;

    public:
        EventCaster(void (Class::*ptr)(EventType const&), Class* instance) :
            cb(std::bind(ptr, instance, std::placeholders::_1)) {}

        void operator ()(Event const& e)
        {
# ifdef DEBUG
            this->cb(dynamic_cast<EventType const&>(e));
# else
            this->cb(reinterpret_cast<EventType const&>(e));
# endif
        }
    };

}}

#endif
