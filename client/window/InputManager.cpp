#include "client/precompiled.hpp"

#include "client/window/InputManager.hpp"
#include "client/window/InputBinder.hpp"

namespace Client { namespace Window {

    InputManager::InputManager(::Client::Window::Window& window, InputBinder* inputBinder) :
        _inputBinder(inputBinder),
        _window(window)
    {
        this->_ResetStates();
    }

    InputManager::~InputManager()
    {
        Tools::Delete(this->_inputBinder);
    }

    void InputManager::_ResetStates()
    {
        for (unsigned int i = 0; i < BindAction::NbBindActions; ++i)
            this->_actionStates[i] = false;
    }

    void InputManager::TriggerAction(InputBinder::Action const& action, BindAction::Type type)
    {
        this->_actionList.push_back(std::make_pair(action, type));
        if (action.string.empty())
            this->_actionStates[action.action] = true;
    }

    void InputManager::TriggerAction(std::string const& action, BindAction::Type type)
    {
        InputBinder::Action a;
        a.string = action;
        this->TriggerAction(a, type);
    }

    void InputManager::TriggerAction(BindAction::BindAction action, BindAction::Type type)
    {
        InputBinder::Action a;
        a.action = action;
        this->TriggerAction(a, type);
    }

}}
