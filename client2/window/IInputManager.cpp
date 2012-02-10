#include "client2/window/IInputManager.hpp"
#include "client2/window/InputBinder.hpp"

namespace Client { namespace Window {

    IInputManager::IInputManager(Client& client, InputBinder* inputBinder) :
        _client(client), _inputBinder(inputBinder)
    {
    }

    IInputManager::~IInputManager()
    {
        delete this->_inputBinder;
    }

    InputBinder& IInputManager::GetInputBinder()
    {
        return *this->_inputBinder;
    }

}}
