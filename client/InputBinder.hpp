#ifndef __CLIENT_INPUTBINDER_HPP__
#define __CLIENT_INPUTBINDER_HPP__

#include <map>
#include <string>

#include "tools/gui/events/Key.hpp"
#include "tools/gui/events/MouseButton.hpp"

namespace Tools { namespace Gui {
    class EventManager;
}}

namespace Client {

    class InputBinder
    {
        private:

        private:
            Tools::Gui::EventManager& _eventManager;
            std::map<std::string, SDLKey> _inputKeys;
            std::map<std::string, Tools::Gui::Events::MouseButton::Button> _inputButtons;
            std::map<SDLKey, std::string> _keyBinds;
            std::map<char, std::string> _asciiBinds;
            std::map<Uint16, std::string> _unicodeBinds;
            std::map<Tools::Gui::Events::MouseButton::Button, std::string> _mouseBinds;
            std::map<std::string, unsigned int> _currentActions;

        public:
            InputBinder(Tools::Gui::EventManager& eventManager);
            bool LoadFile(std::string const& path);
            bool Bind(Uint16 input, std::string const& action);
            bool Bind(std::string const& input, std::string const& action);
            bool Unbind(Uint16 input);
            bool Unbind(std::string const& input);
            unsigned int UnbindAction(std::string const& action);
            std::string GetPrettyKeyName(SDLKey key) const;
            std::string GetPrettyButtonName(Tools::Gui::Events::MouseButton::Button button) const;
        private:
            void _PopulateKeys();
            void _PopulateButtons();
            void _Fire(std::string const& id, Tools::Gui::Event const& ev);
            void _SetInputKey(std::string const& name, SDLKey key);
            void _SetInputButton(std::string const& name, Tools::Gui::Events::MouseButton::Button button);
            void _OnKey(Tools::Gui::Events::Key const& e);
            void _OnMouseButton(Tools::Gui::Events::MouseButton const& e);
            void _OnFrame();
            void _PressAction(std::string const& action);
            void _ReleaseAction(std::string const& action);
    };

}

#endif
