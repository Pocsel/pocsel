#ifndef __SERVER_NETWORK_UDPPACKET_HPP__
#define __SERVER_NETWORK_UDPPACKET_HPP__

#include "common/Packet.hpp"

namespace Server { namespace Network {

    class UdpPacket :
        public Common::Packet
    {
    public:
        bool forceUdp;
    public:
        explicit UdpPacket() : Common::Packet(), forceUdp(false)
        {
        }
        explicit UdpPacket(bool forceUdp) : Common::Packet(), forceUdp(forceUdp)
        {
        }
        virtual ~UdpPacket()
        {
        }
    };

}}

#endif
