#ifndef __CLIENT_CLIENT_HPP__
#define __CLIENT_CLIENT_HPP__

#include <boost/noncopyable.hpp>

namespace Client {

    class Client :
        private boost::noncopyable
    {
        public:
            Client(int ac, char** av);
            int Run();
    };

}

#endif
