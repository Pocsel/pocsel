#ifndef __CLIENT_WINDOW_IINPUTMANAGER_HPP__
#define __CLIENT_WINDOW_IINPUTMANAGER_HPP__

#include "client2/window/InputBinder.hpp"

namespace Client {
    class Client;
}

namespace Client { namespace Window {

    class InputManager :
        private boost::noncopyable
    {
    private:
        InputBinder* _inputBinder;
    protected:
        Client& _client;

    public:
        InputManager(Client& client, InputBinder* inputBinder);
        virtual ~InputManager();
        virtual void ProcessEvents() = 0;
        InputBinder& GetInputBinder();
        void TriggerAction(InputBinder::Action const& action);
        void TriggerAction(std::string const& action);
        void TriggerAction(BindAction::BindAction action);
    };

}}

#endif
