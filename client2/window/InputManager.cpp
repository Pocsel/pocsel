#include "client2/window/InputManager.hpp"
#include "client2/window/InputBinder.hpp"

namespace Client { namespace Window {

    InputManager::InputManager(Client& client, InputBinder* inputBinder) :
        _inputBinder(inputBinder), _client(client)
    {
    }

    InputManager::~InputManager()
    {
        delete this->_inputBinder;
    }

    InputBinder& InputManager::GetInputBinder()
    {
        return *this->_inputBinder;
    }

    void InputManager::TriggerAction(InputBinder::Action const& action)
    {
        action.string.empty() ? this->TriggerAction(action.action) : this->TriggerAction(action.string);
    }

    void InputManager::TriggerAction(std::string const& action)
    {
    }

    void InputManager::TriggerAction(BindAction::BindAction action)
    {
    }

}}
