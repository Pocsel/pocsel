#ifndef __CLIENT_CLIENT_HPP__
#define __CLIENT_CLIENT_HPP__

#include <boost/noncopyable.hpp>

namespace Client {

    class IWindow;

    class Client :
        private boost::noncopyable
    {
        private:
            IWindow* _window;

        public:
            Client(int ac, char** av);
            ~Client();
            int Run();
    };

}

#endif
