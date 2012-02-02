#ifndef __TOOLS_GUI_EVENTS_KEY_HPP__
#define __TOOLS_GUI_EVENTS_KEY_HPP__

#include <SDL/SDL_keysym.h>

#include "tools/enum.hpp"
#include "tools/gui/Event.hpp"

namespace Tools { namespace Gui { namespace Events {

    struct Key : public Tools::Gui::Event
    {
        ENUM Type
        {
            Released,
            Pressed,
            Held,
        };

        Type type;
        SDLKey key;
        SDLMod modifier;
        Uint16 unicode;
        char ascii;
        bool isAscii;

        Key(Type type, bool isAscii, char ascii, Uint16 unicode, SDLKey key, SDLMod modifier) :
            type(type), key(key), modifier(modifier), unicode(unicode), ascii(ascii), isAscii(isAscii)
        {
        }

        bool operator ==(Key const& k)
        {
            return k.type == type && k.key == key && k.modifier == modifier && k.unicode == unicode && k.ascii == ascii && k.isAscii == isAscii;
        }
    };

}}}

#endif
