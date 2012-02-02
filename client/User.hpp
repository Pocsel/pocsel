#ifndef __CLIENT_USER_HPP__
#define __CLIENT_USER_HPP__

#include <boost/noncopyable.hpp>
#include <string>

namespace Common {
    class Packet;
}

namespace Client {

    class Application;

    class User :
        private boost::noncopyable
    {
        private:
            Application& _app;
            std::string _login;
            bool _loggedIn;

        public:
            User(Application& app);
            void Login();
            std::string const& GetLogin() const;
            bool IsLoggedIn() const;
        private:
            void _HandleLoggedIn(Common::Packet& p);
    };

}

#endif
