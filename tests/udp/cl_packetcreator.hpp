#ifndef __CL_PACKETCREATOR_HPP__
#define __CL_PACKETCREATOR_HPP__

#include "common/Packet.hpp"

#include "cl_udppacket.hpp"
#include "tst_protocol.hpp"

namespace cl {

    class PacketCreator
    {
    public:
        static std::unique_ptr<UdpPacket> PassThrough(Uint32 type)
        {
            UdpPacket* p = new UdpPacket();
            p->Write(tst_protocol::ClientToServer::clPassThrough);

            p->Write(type);
            return std::unique_ptr<UdpPacket>(p);
        }

        static std::unique_ptr<Common::Packet> PassThroughOk(Uint32 type)
        {
            Common::Packet* p = new Common::Packet();
            p->Write(tst_protocol::ClientToServer::clPassThroughOk);

            p->Write(type);
            return std::unique_ptr<Common::Packet>(p);
        }
    };

}

#endif
