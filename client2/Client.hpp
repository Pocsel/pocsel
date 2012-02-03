#ifndef __CLIENT_CLIENT_HPP__
#define __CLIENT_CLIENT_HPP__

#include <boost/noncopyable.hpp>

namespace Client {

    class Client :
        private boost::noncopyable
    {
        public:
            int Run();
    };

}

#endif
