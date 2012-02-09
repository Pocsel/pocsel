#ifndef __CLIENT_WINDOW_IINPUTMANAGER_HPP__
#define __CLIENT_WINDOW_IINPUTMANAGER_HPP__

#include <boost/noncopyable.hpp>

namespace Client {
    class Client;
}

namespace Client { namespace Window {

    class IInputManager :
        private boost::noncopyable
    {
    public:
        enum Type
        {
            Pressed,
            Held,
            Released,
            All,
        };

    protected:
        Client& _client;

    public:
        IInputManager(Client& client);
        virtual ~IInputManager();
        virtual void ProcessEvents() = 0;
        void Bind(std::string const& action, Type type, std::function<void(void)> const& callback);
        void Action(std::string const& action, Type type);
    };

}}

#endif
