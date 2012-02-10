#ifndef __CLIENT_WINDOW_IINPUTMANAGER_HPP__
#define __CLIENT_WINDOW_IINPUTMANAGER_HPP__

#include <boost/noncopyable.hpp>

namespace Client {
    class Client;
}

namespace Client { namespace Window {

    class InputBinder;

    class IInputManager :
        private boost::noncopyable
    {
    private:
        InputBinder* _inputBinder;
    protected:
        Client& _client;

    public:
        IInputManager(Client& client, InputBinder* inputBinder);
        virtual ~IInputManager();
        virtual void ProcessEvents() = 0;
        InputBinder& GetInputBinder();
    };

}}

#endif
