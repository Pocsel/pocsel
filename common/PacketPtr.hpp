#ifndef __COMMON_PACKETPTR_HPP__
#define __COMMON_PACKETPTR_HPP__

#include <memory>

namespace Common {

class Packet;

typedef std::unique_ptr<Packet> PacketPtr;

}

#endif
