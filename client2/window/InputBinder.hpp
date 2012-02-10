#ifndef __CLIENT_WINDOW_INPUTBINDER_HPP__
#define __CLIENT_WINDOW_INPUTBINDER_HPP__

#include <boost/noncopyable.hpp>

#include "client2/window/Inputs.hpp"
#include "client2/BindAction.hpp"

namespace Client { namespace Window {

    class InputBinder :
        private boost::noncopyable
    {
        public:
            struct Action
            {
                std::string string;
                BindAction::BindAction action;

                Action();
                bool operator ==(Action const&) const;
            };

        private:
            std::map<std::string, SpecialKey::SpecialKey> _specialKeys;
            std::map<std::string, Button::Button> _buttons;
            std::map<std::string, BindAction::BindAction> _actions;
            std::map<SpecialKey::SpecialKey, Action> _specialKeyBinds;
            std::map<char, Action> _asciiBinds;
            std::map<Uint16, Action> _unicodeBinds;
            std::map<Button::Button, Action> _mouseBinds;

        public:
            InputBinder();
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
            void _Bind(Button::Button button, std::string const& action);
            void _Bind(SpecialKey::SpecialKey specialKey, std::string const& action);
            void _Bind(char ascii, std::string const& action);
            void _Bind(Uint16 unicode, std::string const& action);
            void _PopulateSpecialKeys();
            void _PopulateButtons();
            void _PopulateActions();
            void _StringToAction(std::string const& str, Action& action) const;
    };

}}

#endif
