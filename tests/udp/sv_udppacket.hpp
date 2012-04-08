#ifndef __SV_UDPPACKET_HPP__
#define __SV_UDPPACKET_HPP__

#include "common/Packet.hpp"

namespace sv {

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

}

#endif

