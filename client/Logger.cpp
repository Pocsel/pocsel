#ifndef __CLIENT_CHUNK_HPP__
#define __CLIENT_CHUNK_HPP__

#include "client/Logger.hpp"

namespace Client {

    _CoutLogger cout(std::cout, "client.log");
    _DebugLogger debug(std::cout, "debug.log");

}
#endif
