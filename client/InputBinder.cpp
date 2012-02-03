#include <boost/algorithm/string.hpp>

#include "client/InputBinder.hpp"

#include "tools/gui/EventManager.hpp"
#include "tools/gui/EventCaster.hpp"
#include "tools/gui/events/Key.hpp"
#include "tools/gui/events/MouseButton.hpp"
#include "tools/gui/events/BindAction.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Bind.hpp"

namespace Client {

    InputBinder::InputBinder(Tools::Gui::EventManager& eventManager) :
        _eventManager(eventManager)
    {
        this->_PopulateKeys();
        this->_PopulateButtons();
        this->_eventManager.Connect(
             "on-key",
             Tools::Gui::EventCaster<Tools::Gui::Events::Key, InputBinder>(&InputBinder::_OnKey, this)
        );
        this->_eventManager.Connect(
            "on-mouse-button",
            Tools::Gui::EventCaster<Tools::Gui::Events::MouseButton, InputBinder>(&InputBinder::_OnMouseButton, this)
        );
        this->_eventManager.Connect(
            "on-frame", std::bind(&InputBinder::_OnFrame, this)
        );
    }

    bool InputBinder::LoadFile(std::string const& path)
    {
        typedef bool (InputBinder::*MethodType)(std::string const&, std::string const&);
        try
        {
            std::cout << "Loading bind file \"" << path << "\"." << std::endl;
            Tools::Lua::Interpreter i;
            i.Bind("bind",
                   static_cast<MethodType>(&InputBinder::Bind), this, std::placeholders::_1, std::placeholders::_2
            );
            i.ExecFile(path);
        }
        catch (std::exception& e)
        {
            std::cout << "Failed to load bind file \"" << path << "\": " << e.what() << std::endl;
            return false;
        }
        std::cout << "Bind file \"" << path << "\" successfully loaded." << std::endl;
        return true;
    }

    bool InputBinder::Bind(Uint16 input, std::string const& action)
    {
        if (action.empty())
            return false;
        this->_unicodeBinds[input] = action;
#ifdef DEBUG
        std::cout << "Unicode key " << static_cast<int>(input) << " bound to \"" << action << "\"." << std::endl;
#endif
        return true;
    }

    bool InputBinder::Bind(std::string const& input, std::string const& action)
    {
        if (action.empty())
            return false;
        std::string lowerInput = input;
        boost::to_lower(lowerInput);
        if (lowerInput.size() == 1)
        {
            this->_asciiBinds[lowerInput[0]] = action;
#ifdef DEBUG
            std::cout << "Ascii key '" << input << "' bound to \"" << action << "\"." << std::endl;
#endif
            return true;
        }
        {
            auto it = this->_inputKeys.find(lowerInput);
            if (it != this->_inputKeys.end())
            {
                this->_keyBinds[it->second] = action;
#ifdef DEBUG
                std::cout << "Special key \"" << input << "\" bound to \"" << action << "\"." << std::endl;
#endif
                return true;
            }
        }
        {
            auto it = this->_inputButtons.find(lowerInput);
            if (it != this->_inputButtons.end())
            {
                this->_mouseBinds[it->second] = action;
#ifdef DEBUG
                std::cout << "Mouse button \"" << input << "\" bound to \"" << action << "\"." << std::endl;
#endif
                return true;
            }
        }
        return false;
    }

    bool InputBinder::Unbind(Uint16 input)
    {
#ifdef DEBUG
        std::cout << "Unicode key " << static_cast<int>(input) << " unbound." << std::endl;
#endif
        return this->_unicodeBinds.erase(input) == 1;
    }

    bool InputBinder::Unbind(std::string const& input)
    {
#ifdef DEBUG
        std::cout << "Key \"" << input << "\" unbound." << std::endl;
#endif
        std::string lowerInput = input;
        boost::to_lower(lowerInput);
        if (lowerInput.size() == 1)
            return this->_asciiBinds.erase(lowerInput[0]) == 1;
        {
            auto it = this->_inputKeys.find(lowerInput);
            if (it != this->_inputKeys.end())
                return this->_keyBinds.erase(it->second) == 1;
        }
        {
            auto it = this->_inputButtons.find(lowerInput);
            if (it != this->_inputButtons.end())
                return this->_mouseBinds.erase(it->second) == 1;
        }
        return false;
    }

    unsigned int InputBinder::UnbindAction(std::string const& action)
    {
        unsigned int nb = 0;
        {
            auto it = this->_asciiBinds.begin();
            auto itEnd = this->_asciiBinds.end();
            for (; it != itEnd; ++it)
                if (it->second == action)
                {
                    this->_asciiBinds.erase(it);
                    ++nb;
                }
        }
        {
            auto it = this->_keyBinds.begin();
            auto itEnd = this->_keyBinds.end();
            for (; it != itEnd; ++it)
                if (it->second == action)
                {
                    this->_keyBinds.erase(it);
                    ++nb;
                }
        }
        {
            auto it = this->_unicodeBinds.begin();
            auto itEnd = this->_unicodeBinds.end();
            for (; it != itEnd; ++it)
                if (it->second == action)
                {
                    this->_unicodeBinds.erase(it);
                    ++nb;
                }
        }
        {
            auto it = this->_mouseBinds.begin();
            auto itEnd = this->_mouseBinds.end();
            for (; it != itEnd; ++it)
                if (it->second == action)
                {
                    this->_mouseBinds.erase(it);
                    ++nb;
                }
        }
#ifdef DEBUG
        if (nb > 0)
            std::cout << "Action \"" << action << "\" unbound." << std::endl;
#endif
        return nb;
    }

    std::string InputBinder::GetPrettyKeyName(SDLKey key) const
    {
        auto it = this->_inputKeys.begin();
        auto itEnd = this->_inputKeys.end();
        for (; it != itEnd; ++it)
            if (it->second == key)
                return it->first;
        std::string ret;
        if (::isprint(key) && !::isspace(key))
            ret += static_cast<char>(key);
        else
            ret = "key" + static_cast<int>(key);
        return ret;
    }

    std::string InputBinder::GetPrettyButtonName(Tools::Gui::Events::MouseButton::Button button) const
    {
        auto it = this->_inputButtons.begin();
        auto itEnd = this->_inputButtons.end();
        for (; it != itEnd; ++it)
            if (it->second == button)
                return it->first;
        return "mouse" + static_cast<int>(button);
    }

    void InputBinder::_OnFrame()
    {
        auto it = this->_currentActions.begin();
        auto itEnd = this->_currentActions.end();
        for (; it != itEnd; ++it)
            this->_Fire("bind/" + it->first, Tools::Gui::Events::BindAction(Tools::Gui::Events::BindAction::Type::Held));
    }

    void InputBinder::_OnKey(Tools::Gui::Events::Key const& e)
    {
        if (e.type == Tools::Gui::Events::Key::Type::Held)
            return;
        std::string action;
        std::map<char, std::string>::iterator itAsciiBind;
        std::map<Uint16, std::string>::iterator itUnicodeBind;
        std::map<SDLKey, std::string>::iterator itKeyBind;
        if (e.isAscii && (itAsciiBind = this->_asciiBinds.find(e.ascii)) != this->_asciiBinds.end())
            action = itAsciiBind->second;
        else if (e.unicode > 0 && (itUnicodeBind = this->_unicodeBinds.find(e.unicode)) != this->_unicodeBinds.end())
            action = itUnicodeBind->second;
        else if ((itKeyBind = this->_keyBinds.find(e.key)) != this->_keyBinds.end())
            action = itKeyBind->second;
        if (action.empty())
            return;
        if (e.type == Tools::Gui::Events::Key::Type::Released)
            this->_ReleaseAction(action);
        else
            this->_PressAction(action);
    }

    void InputBinder::_OnMouseButton(Tools::Gui::Events::MouseButton const& e)
    {
        if (e.type == Tools::Gui::Events::MouseButton::Type::Held)
            return;
        auto itBind = this->_mouseBinds.find(e.button);
        if (itBind == this->_mouseBinds.end())
            return;
        if (e.type == Tools::Gui::Events::MouseButton::Type::Released)
            this->_ReleaseAction(itBind->second);
        else
            this->_PressAction(itBind->second);
    }

    void InputBinder::_ReleaseAction(std::string const& action)
    {
        auto itAction = this->_currentActions.find(action);
        if (itAction != this->_currentActions.end())
        {
            --itAction->second;
            if (!itAction->second)
            {
                this->_Fire("bind/" + action, Tools::Gui::Events::BindAction(Tools::Gui::Events::BindAction::Type::Released));
                this->_currentActions.erase(itAction);
            }
        }
    }

    void InputBinder::_PressAction(std::string const& action)
    {
        auto itAction = this->_currentActions.find(action);
        if (itAction == this->_currentActions.end())
        {
            this->_Fire("bind/" + action, Tools::Gui::Events::BindAction(Tools::Gui::Events::BindAction::Type::Pressed));
            this->_currentActions[action] = 1;
        }
        else
            ++itAction->second;
    }

    void InputBinder::_Fire(std::string const& id, Tools::Gui::Event const& ev)
    {
        this->_eventManager.Fire(id, ev);
    }

    void InputBinder::_PopulateKeys()
    {
        this->_SetInputKey("Backspace", SDLK_BACKSPACE);
        this->_SetInputKey("Tab", SDLK_TAB);
        this->_SetInputKey("Clear", SDLK_CLEAR);
        this->_SetInputKey("Return", SDLK_RETURN);
        this->_SetInputKey("Pause", SDLK_PAUSE);
        this->_SetInputKey("Escape", SDLK_ESCAPE);
        this->_SetInputKey("Space", SDLK_SPACE);
        this->_SetInputKey("Delete", SDLK_DELETE);
        this->_SetInputKey("Kp0", SDLK_KP0);
        this->_SetInputKey("Kp1", SDLK_KP1);
        this->_SetInputKey("Kp2", SDLK_KP2);
        this->_SetInputKey("Kp3", SDLK_KP3);
        this->_SetInputKey("Kp4", SDLK_KP4);
        this->_SetInputKey("Kp5", SDLK_KP5);
        this->_SetInputKey("Kp6", SDLK_KP6);
        this->_SetInputKey("Kp7", SDLK_KP7);
        this->_SetInputKey("Kp8", SDLK_KP8);
        this->_SetInputKey("Kp9", SDLK_KP9);
        this->_SetInputKey("KpPeriod", SDLK_KP_PERIOD);
        this->_SetInputKey("KpDivide", SDLK_KP_DIVIDE);
        this->_SetInputKey("KpMultiply", SDLK_KP_MULTIPLY);
        this->_SetInputKey("KpMinus", SDLK_KP_MINUS);
        this->_SetInputKey("KpPlus", SDLK_KP_PLUS);
        this->_SetInputKey("KpEnter", SDLK_KP_ENTER);
        this->_SetInputKey("KpEquals", SDLK_KP_EQUALS);
        this->_SetInputKey("Up", SDLK_UP);
        this->_SetInputKey("Down", SDLK_DOWN);
        this->_SetInputKey("Right", SDLK_RIGHT);
        this->_SetInputKey("Left", SDLK_LEFT);
        this->_SetInputKey("Insert", SDLK_INSERT);
        this->_SetInputKey("Home", SDLK_HOME);
        this->_SetInputKey("End", SDLK_END);
        this->_SetInputKey("Pageup", SDLK_PAGEUP);
        this->_SetInputKey("Pagedown", SDLK_PAGEDOWN);
        this->_SetInputKey("F1", SDLK_F1);
        this->_SetInputKey("F2", SDLK_F2);
        this->_SetInputKey("F3", SDLK_F3);
        this->_SetInputKey("F4", SDLK_F4);
        this->_SetInputKey("F5", SDLK_F5);
        this->_SetInputKey("F6", SDLK_F6);
        this->_SetInputKey("F7", SDLK_F7);
        this->_SetInputKey("F8", SDLK_F8);
        this->_SetInputKey("F9", SDLK_F9);
        this->_SetInputKey("F10", SDLK_F10);
        this->_SetInputKey("F11", SDLK_F11);
        this->_SetInputKey("F12", SDLK_F12);
        this->_SetInputKey("F13", SDLK_F13);
        this->_SetInputKey("F14", SDLK_F14);
        this->_SetInputKey("F15", SDLK_F15);
        this->_SetInputKey("Numlock", SDLK_NUMLOCK);
        this->_SetInputKey("Capslock", SDLK_CAPSLOCK);
        this->_SetInputKey("Scrollock", SDLK_SCROLLOCK);
        this->_SetInputKey("Rshift", SDLK_RSHIFT);
        this->_SetInputKey("Lshift", SDLK_LSHIFT);
        this->_SetInputKey("Rctrl", SDLK_RCTRL);
        this->_SetInputKey("Lctrl", SDLK_LCTRL);
        this->_SetInputKey("Ralt", SDLK_RALT);
        this->_SetInputKey("Lalt", SDLK_LALT);
        this->_SetInputKey("Rmeta", SDLK_RMETA);
        this->_SetInputKey("Lmeta", SDLK_LMETA);
        this->_SetInputKey("Rwindows", SDLK_RMETA);  // huhuhu
        this->_SetInputKey("Lwindows", SDLK_LMETA);  //
        this->_SetInputKey("Rlinux", SDLK_RMETA);    //
        this->_SetInputKey("Llinux", SDLK_LMETA);    //
        this->_SetInputKey("Lsuper", SDLK_LSUPER);
        this->_SetInputKey("Rsuper", SDLK_RSUPER);
        this->_SetInputKey("Mode", SDLK_MODE);
        this->_SetInputKey("AltGr", SDLK_MODE); // added for simplicity
        this->_SetInputKey("Compose", SDLK_COMPOSE);
        this->_SetInputKey("Help", SDLK_HELP);
        this->_SetInputKey("Print", SDLK_PRINT);
        this->_SetInputKey("Sysreq", SDLK_SYSREQ);
        this->_SetInputKey("Break", SDLK_BREAK);
        this->_SetInputKey("Menu", SDLK_MENU);
        this->_SetInputKey("Power", SDLK_POWER);
        this->_SetInputKey("Euro", SDLK_EURO);
        this->_SetInputKey("Undo", SDLK_UNDO);
    }

    void InputBinder::_PopulateButtons()
    {
        this->_SetInputButton("LeftClick", Tools::Gui::Events::MouseButton::Button::Left);
        this->_SetInputButton("MiddleClick", Tools::Gui::Events::MouseButton::Button::Middle);
        this->_SetInputButton("RightClick", Tools::Gui::Events::MouseButton::Button::Right);
        this->_SetInputButton("WheelUp", Tools::Gui::Events::MouseButton::Button::WheelUp);
        this->_SetInputButton("WheelDown", Tools::Gui::Events::MouseButton::Button::WheelDown);
        this->_SetInputButton("X1Click", Tools::Gui::Events::MouseButton::Button::Special1);
        this->_SetInputButton("X2Click", Tools::Gui::Events::MouseButton::Button::Special2);
    }

    void InputBinder::_SetInputKey(std::string const& name, SDLKey key)
    {
        std::string lowerName = name;
        boost::to_lower(lowerName);
        this->_inputKeys[lowerName] = key;
    }

    void InputBinder::_SetInputButton(std::string const& name, Tools::Gui::Events::MouseButton::Button button)
    {
        std::string lowerName = name;
        boost::to_lower(lowerName);
        this->_inputButtons[lowerName] = button;
    }

}
