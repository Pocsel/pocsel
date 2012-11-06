#ifndef __TOOLS_WINDOW_INPUTBINDER_HPP__
#define __TOOLS_WINDOW_INPUTBINDER_HPP__

#include "tools/window/Inputs.hpp"
#include "tools/window/BindAction.hpp"

namespace Luasel {
    class CallHelper;
}

namespace Tools { namespace Window {

    class InputBinder :
        private boost::noncopyable
    {
        public:
            struct Action
            {
                std::string string;
                Tools::Window::BindAction::BindAction action;

                Action();
                bool operator ==(Action const&) const;
            };

        private:
            std::map<std::string, SpecialKey::SpecialKey> _specialKeys;
            std::map<std::string, Button::Button> _buttons;
            std::map<std::string, Tools::Window::BindAction::BindAction> _actions;
            std::map<SpecialKey::SpecialKey, Action> _specialKeyBinds;
            std::map<char, Action> _asciiBinds;
            std::map<Uint16, Action> _unicodeBinds;
            std::map<Button::Button, Action> _mouseBinds;

        public:
            InputBinder(std::map<std::string, Tools::Window::BindAction::BindAction> const& actions);
            virtual ~InputBinder();
            bool LoadFile(std::string const& path);
            bool Bind(std::string const& input, std::string const& action);
            bool Unbind(std::string const& input);
            unsigned int UnbindAction(std::string const& action);
            bool GetAsciiAction(char ascii, Action& action) const;
            bool GetUnicodeAction(Uint16 unicode, Action& action) const;
            bool GetSpecialKeyAction(SpecialKey::SpecialKey specialKey, Action& action) const;
            bool GetButtonAction(Button::Button button, Action& action) const;
        private:
            void _BindFromLua(Luasel::CallHelper& callHelper);
            void _Bind(Button::Button button, std::string const& action);
            void _Bind(SpecialKey::SpecialKey specialKey, std::string const& action);
            void _Bind(char ascii, std::string const& action);
            void _Bind(Uint16 unicode, std::string const& action);
            void _PopulateSpecialKeys();
            void _PopulateButtons();
            void _StringToAction(std::string const& str, Action& action) const;
    };

}}

#endif
