#include "client2/window/IInputManager.hpp"

namespace Client { namespace Window {

    IInputManager::IInputManager(Client& client) :
        _client(client)
    {
    }

    IInputManager::~IInputManager()
    {
    }

}}
