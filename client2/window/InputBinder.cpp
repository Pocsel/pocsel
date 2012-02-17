#include <boost/algorithm/string.hpp>

#include "client2/window/InputBinder.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Bind.hpp"

namespace Client { namespace Window {

    InputBinder::InputBinder()
    {
        this->_PopulateActions();
        this->_PopulateSpecialKeys();
        this->_PopulateButtons();
    }

    InputBinder::~InputBinder()
    {
    }

    void InputBinder::_PopulateActions()
    {
        this->_actions["quit"] = BindAction::Quit;
        this->_actions["menu"] = BindAction::Menu;
        this->_actions["forward"] = BindAction::Forward;
        this->_actions["backward"] = BindAction::Backward;
        this->_actions["left"] = BindAction::Left;
        this->_actions["right"] = BindAction::Right;
        this->_actions["jump"] = BindAction::Jump;
        this->_actions["fire"] = BindAction::Fire;
        this->_actions["secondaryfire"] = BindAction::SecondaryFire;
        this->_actions["use"] = BindAction::Use;
        this->_actions["test"] = BindAction::Test;
    }

    InputBinder::Action::Action() :
        action(BindAction::None)
    {
    }

    bool InputBinder::Action::operator ==(Action const& action) const
    {
        return this->action == action.action && this->string == action.string;
    }

    void InputBinder::_StringToAction(std::string const& str, Action& action) const
    {
        std::string lowerStr = str;
        boost::to_lower(lowerStr);
        auto it = this->_actions.find(lowerStr);
        if (it != this->_actions.end())
        {
            action.action = it->second;
            action.string.clear();
        }
        else
        {
            action.action = BindAction::None;
            action.string = str;
        }
    }

    bool InputBinder::GetAsciiAction(char ascii, Action& action) const
    {
        auto it = this->_asciiBinds.find(ascii);
        if (it != this->_asciiBinds.end())
        {
            action = it->second;
            return true;
        }
        return false;
    }

    bool InputBinder::GetUnicodeAction(Uint16 unicode, Action& action) const
    {
        auto it = this->_unicodeBinds.find(unicode);
        if (it != this->_unicodeBinds.end())
        {
            action = it->second;
            return true;
        }
        return false;
    }

    bool InputBinder::GetSpecialKeyAction(SpecialKey::SpecialKey specialKey, Action& action) const
    {
        auto it = this->_specialKeyBinds.find(specialKey);
        if (it != this->_specialKeyBinds.end())
        {
            action = it->second;
            return true;
        }
        return false;
    }

    bool InputBinder::GetButtonAction(Button::Button button, Action& action) const
    {
        auto it = this->_mouseBinds.find(button);
        if (it != this->_mouseBinds.end())
        {
            action = it->second;
            return true;
        }
        return false;
    }

    bool InputBinder::LoadFile(std::string const& path)
    {
        try
        {
            Tools::Lua::Interpreter i;
            i.Bind("bind", &InputBinder::Bind, this, std::placeholders::_1, std::placeholders::_2);
            i.ExecFile(path);
        }
        catch (std::exception& e)
        {
            Tools::error << "Failed to load bindings file \"" << path << "\": " << e.what() << Tools::endl;
            return false;
        }
        Tools::debug << "Bindings file \"" << path << "\" successfully loaded.\n";
        return true;
    }

    bool InputBinder::Bind(std::string const& input, std::string const& action)
    {
        if (action.empty())
            return false;
        std::string lowerInput = input;
        boost::to_lower(lowerInput);
        Action a;
        this->_StringToAction(action, a);
        // ascii
        if (lowerInput.size() == 1)
        {
            this->_asciiBinds[lowerInput[0]] = a;
            Tools::debug << "Ascii key '" << input << "' bound to \"" << action << "\".\n";
            return true;
        }
        // unicode
        // TODO Here: if unicode -> add to _unicodeBinds and return true
        // special keys
        {
            auto it = this->_specialKeys.find(lowerInput);
            if (it != this->_specialKeys.end())
            {
                this->_specialKeyBinds[it->second] = a;
                Tools::debug << "Special key \"" << input << "\" bound to \"" << action << "\".\n";
                return true;
            }
        }
        // mouse buttons
        {
            auto it = this->_buttons.find(lowerInput);
            if (it != this->_buttons.end())
            {
                this->_mouseBinds[it->second] = a;
                Tools::debug << "Mouse button \"" << input << "\" bound to \"" << action << "\".\n";
                return true;
            }
        }
        return false;
    }

    bool InputBinder::Unbind(std::string const& input)
    {
        Tools::debug << "Key \"" << input << "\" unbound.\n";
        std::string lowerInput = input;
        boost::to_lower(lowerInput);
        // ascii
        if (lowerInput.size() == 1)
            return this->_asciiBinds.erase(lowerInput[0]) == 1;
        // unicode
        // TODO Here: if unicode -> remove from _unicodeBinds and return true
        // special keys
        {
            auto it = this->_specialKeys.find(lowerInput);
            if (it != this->_specialKeys.end())
                return this->_specialKeyBinds.erase(it->second) == 1;
        }
        // mouse buttons
        {
            auto it = this->_buttons.find(lowerInput);
            if (it != this->_buttons.end())
                return this->_mouseBinds.erase(it->second) == 1;
        }
        return false;
    }

    unsigned int InputBinder::UnbindAction(std::string const& action)
    {
        Action a;
        this->_StringToAction(action, a);
        unsigned int nb = 0;
        // ascii
        {
            auto it = this->_asciiBinds.begin();
            auto itEnd = this->_asciiBinds.end();
            for (; it != itEnd; ++it)
                if (it->second == a)
                {
                    this->_asciiBinds.erase(it);
                    ++nb;
                }
        }
        // unicode
        {
            auto it = this->_unicodeBinds.begin();
            auto itEnd = this->_unicodeBinds.end();
            for (; it != itEnd; ++it)
                if (it->second == a)
                {
                    this->_unicodeBinds.erase(it);
                    ++nb;
                }
        }
        // special keys
        {
            auto it = this->_specialKeyBinds.begin();
            auto itEnd = this->_specialKeyBinds.end();
            for (; it != itEnd; ++it)
                if (it->second == a)
                {
                    this->_specialKeyBinds.erase(it);
                    ++nb;
                }
        }
        // mouse buttons
        {
            auto it = this->_mouseBinds.begin();
            auto itEnd = this->_mouseBinds.end();
            for (; it != itEnd; ++it)
                if (it->second == a)
                {
                    this->_mouseBinds.erase(it);
                    ++nb;
                }
        }
        if (nb > 0)
            Tools::debug << "Action \"" << action << "\" unbound.\n";
        return nb;
    }

    void InputBinder::_PopulateButtons()
    {
        this->_buttons["left"] = Button::Left;
        this->_buttons["middle"] = Button::Middle;
        this->_buttons["right"] = Button::Right;
        this->_buttons["wheelup"] = Button::WheelUp;
        this->_buttons["wheeldown"] = Button::WheelDown;
        this->_buttons["special1"] = Button::Special1;
        this->_buttons["special2"] = Button::Special2;
    }

    void InputBinder::_PopulateSpecialKeys()
    {
        this->_specialKeys["backspace"] = SpecialKey::Backspace;
        this->_specialKeys["tab"] = SpecialKey::Tab;
        this->_specialKeys["clear"] = SpecialKey::Clear;
        this->_specialKeys["return"] = SpecialKey::Return;
        this->_specialKeys["pause"] = SpecialKey::Pause;
        this->_specialKeys["escape"] = SpecialKey::Escape;
        this->_specialKeys["space"] = SpecialKey::Space;
        this->_specialKeys["delete"] = SpecialKey::Delete;
        this->_specialKeys["kp0"] = SpecialKey::Kp0;
        this->_specialKeys["kp1"] = SpecialKey::Kp1;
        this->_specialKeys["kp2"] = SpecialKey::Kp2;
        this->_specialKeys["kp3"] = SpecialKey::Kp3;
        this->_specialKeys["kp4"] = SpecialKey::Kp4;
        this->_specialKeys["kp5"] = SpecialKey::Kp5;
        this->_specialKeys["kp6"] = SpecialKey::Kp6;
        this->_specialKeys["kp7"] = SpecialKey::Kp7;
        this->_specialKeys["kp8"] = SpecialKey::Kp8;
        this->_specialKeys["kp9"] = SpecialKey::Kp9;
        this->_specialKeys["kpperiod"] = SpecialKey::KpPeriod;
        this->_specialKeys["kpdivide"] = SpecialKey::KpDivide;
        this->_specialKeys["kpmultiply"] = SpecialKey::KpMultiply;
        this->_specialKeys["kpminus"] = SpecialKey::KpMinus;
        this->_specialKeys["kpplus"] = SpecialKey::KpPlus;
        this->_specialKeys["kpenter"] = SpecialKey::KpEnter;
        this->_specialKeys["kpequals"] = SpecialKey::KpEquals;
        this->_specialKeys["up"] = SpecialKey::Up;
        this->_specialKeys["down"] = SpecialKey::Down;
        this->_specialKeys["right"] = SpecialKey::Right;
        this->_specialKeys["left"] = SpecialKey::Left;
        this->_specialKeys["insert"] = SpecialKey::Insert;
        this->_specialKeys["home"] = SpecialKey::Home;
        this->_specialKeys["end"] = SpecialKey::End;
        this->_specialKeys["pageup"] = SpecialKey::Pageup;
        this->_specialKeys["pagedown"] = SpecialKey::Pagedown;
        this->_specialKeys["f1"] = SpecialKey::F1;
        this->_specialKeys["f2"] = SpecialKey::F2;
        this->_specialKeys["f3"] = SpecialKey::F3;
        this->_specialKeys["f4"] = SpecialKey::F4;
        this->_specialKeys["f5"] = SpecialKey::F5;
        this->_specialKeys["f6"] = SpecialKey::F6;
        this->_specialKeys["f7"] = SpecialKey::F7;
        this->_specialKeys["f8"] = SpecialKey::F8;
        this->_specialKeys["f9"] = SpecialKey::F9;
        this->_specialKeys["f10"] = SpecialKey::F10;
        this->_specialKeys["f11"] = SpecialKey::F11;
        this->_specialKeys["f12"] = SpecialKey::F12;
        this->_specialKeys["f13"] = SpecialKey::F13;
        this->_specialKeys["f14"] = SpecialKey::F14;
        this->_specialKeys["f15"] = SpecialKey::F15;
        this->_specialKeys["numlock"] = SpecialKey::Numlock;
        this->_specialKeys["capslock"] = SpecialKey::Capslock;
        this->_specialKeys["scrollock"] = SpecialKey::Scrollock;
        this->_specialKeys["rshift"] = SpecialKey::Rshift;
        this->_specialKeys["lshift"] = SpecialKey::Lshift;
        this->_specialKeys["rctrl"] = SpecialKey::Rctrl;
        this->_specialKeys["lctrl"] = SpecialKey::Lctrl;
        this->_specialKeys["ralt"] = SpecialKey::Ralt;
        this->_specialKeys["lalt"] = SpecialKey::Lalt;
        this->_specialKeys["rmeta"] = SpecialKey::Rmeta;
        this->_specialKeys["lmeta"] = SpecialKey::Lmeta;
        this->_specialKeys["rwindows"] = SpecialKey::Rmeta; // bonus
        this->_specialKeys["lwindows"] = SpecialKey::Lmeta; // bonus
        this->_specialKeys["rlinux"] = SpecialKey::Rmeta; // bonus
        this->_specialKeys["llinux"] = SpecialKey::Lmeta; // bonus
        this->_specialKeys["lsuper"] = SpecialKey::Lsuper;
        this->_specialKeys["rsuper"] = SpecialKey::Rsuper;
        this->_specialKeys["altgr"] = SpecialKey::AltGr;
        this->_specialKeys["mode"] = SpecialKey::AltGr; // bonus
        this->_specialKeys["compose"] = SpecialKey::Compose;
        this->_specialKeys["help"] = SpecialKey::Help;
        this->_specialKeys["print"] = SpecialKey::Print;
        this->_specialKeys["sysreq"] = SpecialKey::Sysreq;
        this->_specialKeys["break"] = SpecialKey::Break;
        this->_specialKeys["menu"] = SpecialKey::Menu;
        this->_specialKeys["power"] = SpecialKey::Power;
        this->_specialKeys["euro"] = SpecialKey::Euro;
        this->_specialKeys["undo"] = SpecialKey::Undo;
    }

}}
