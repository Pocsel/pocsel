#include <SDL/SDL.h>

#include "events/all.hpp"
#include "EventManagerSdl.hpp"
#include "Window.hpp"
#include "WindowSdl.hpp"

namespace Tools { namespace Gui {

    EventManagerImplem::EventManagerImplem(Window& window) :
        _window(window)
    {
    }

    unsigned int EventManagerImplem::Poll()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_VIDEOEXPOSE)
                this->_Fire("on-expose", Events::Expose(this->_window.GetImplem().GetWidth(), this->_window.GetImplem().GetHeight()));
            else if (e.type == SDL_MOUSEMOTION)
            {
                this->_Fire("on-mouse-move", Events::MouseMove(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel));
                auto it = this->_buttonsHeld.begin();
                auto itEnd = this->_buttonsHeld.end();
                for (; it != itEnd; ++it)
                {
                    it->x = e.motion.x;
                    it->y = e.motion.y;
                }
            }
            else if (e.type == SDL_KEYDOWN)
            {
                char ascii = 0;
                bool isAscii = false;
                if (e.key.keysym.unicode != 0 && (e.key.keysym.unicode & 0xFF80) == 0)
                {
                    ascii = e.key.keysym.unicode & 0x7F;
                    isAscii = true;
                }
                Events::Key event(Events::Key::Type::Pressed, isAscii, ascii, e.key.keysym.unicode, e.key.keysym.sym, e.key.keysym.mod);
                this->_Fire("on-key", event);
                event.type = Events::Key::Type::Held;
                this->_keysHeld.push_back(event);
            }
            else if (e.type == SDL_KEYUP)
            {
                bool found = false;
                auto it = this->_keysHeld.begin();
                auto itEnd = this->_keysHeld.end();
                for (; it != itEnd; ++it)
                    if (it->key == e.key.keysym.sym)
                    {
                        this->_Fire("on-key", Events::Key(Events::Key::Type::Released, it->isAscii, it->ascii, it->unicode, e.key.keysym.sym, e.key.keysym.mod));
                        found = true;
                        this->_keysHeld.erase(it);
                        break;
                    }
                if (!found)
                    this->_Fire("on-key", Events::Key(Events::Key::Type::Released, false, 0, 0, e.key.keysym.sym, e.key.keysym.mod));
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                Events::MouseButton event(Events::MouseButton::Type::Pressed, static_cast<Events::MouseButton::Button>(e.button.button), e.button.x, e.button.y);
                this->_Fire("on-mouse-button", event);
                event.type = Events::MouseButton::Type::Held;
                this->_buttonsHeld.push_back(event);
            }
            else if (e.type == SDL_MOUSEBUTTONUP)
            {
                this->_Fire("on-mouse-button", Events::MouseButton(Events::MouseButton::Type::Released, static_cast<Events::MouseButton::Button>(e.button.button), e.button.x, e.button.y));
                auto it = this->_buttonsHeld.begin();
                auto itEnd = this->_buttonsHeld.end();
                for (; it != itEnd; ++it)
                    if (static_cast<Uint8>(it->button) == e.button.button)
                    {
                        this->_buttonsHeld.erase(it);
                        break;
                    }
            }
            else if (e.type == SDL_VIDEORESIZE)
            {
                if (e.resize.w > 0 && e.resize.w < 10000 && e.resize.h > 0 && e.resize.h < 10000)
                {
                    Vector2<size_t> size(e.resize.w, e.resize.h);
                    this->_window.GetImplem().Resize(size);
                    this->_Fire("on-resize", Events::Resize(size.w, size.h));
                }
            }
            else if (e.type == SDL_QUIT)
                this->_Fire("on-quit", Events::Quit());
        }
        // repeat keys
        {
            this->_keysHeld.unique();
            auto it = this->_keysHeld.begin();
            auto itEnd = this->_keysHeld.end();
            for (; it != itEnd; ++it)
                this->_Fire("on-key", *it);
        }
        // repeat buttons
        {
            this->_buttonsHeld.unique();
            auto it = this->_buttonsHeld.begin();
            auto itEnd = this->_buttonsHeld.end();
            for (; it != itEnd; ++it)
                this->_Fire("on-mouse-button", *it);
        }
        return 0;
    }

    void EventManagerImplem::Stop()
    {
    }

    void EventManagerImplem::_Fire(std::string const& id, Event const& ev)
    {
        this->_window.GetEventManager().Fire(id, ev);
    }

}}
