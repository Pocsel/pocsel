#ifndef __TOOLS_GUI_EVENTS_MOUSEBUTTON_HPP__
#define __TOOLS_GUI_EVENTS_MOUSEBUTTON_HPP__

#include <SDL/SDL_mouse.h>

#include "tools/enum.hpp"
#include "tools/gui/Event.hpp"

namespace Tools { namespace Gui { namespace Events {

    struct MouseButton : public Tools::Gui::Event
    {
        ENUM Type
        {
            Released,
            Pressed,
            Held,
        };

        ENUM Button
        {
            Left = SDL_BUTTON_LEFT,
            Right = SDL_BUTTON_RIGHT,
            Middle = SDL_BUTTON_MIDDLE,
            WheelUp = SDL_BUTTON_WHEELUP,
            WheelDown = SDL_BUTTON_WHEELDOWN,
            Special1 = SDL_BUTTON_X1,
            Special2 = SDL_BUTTON_X2,
        };

        Type type;
        Button button;
        int x;
        int y;

        MouseButton(Type type, Button button, int x, int y) :
            type(type), button(button), x(x), y(y)
        {
        }

        bool operator ==(MouseButton const& b)
        {
            // pas de test sur x et y, c'est fait expres
            return b.type == type && b.button == button;
        }
    };

}}}

#endif
