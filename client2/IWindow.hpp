#ifndef __CLIENT_IWINDOW_HPP__
#define __CLIENT_IWINDOW_HPP__

#include <boost/noncopyable.hpp>

namespace Client {

    class IWindow :
        private boost::noncopyable
    {
        public:
            virtual ~IWindow();
            virtual void Render() = 0;
    };

}

#endif
