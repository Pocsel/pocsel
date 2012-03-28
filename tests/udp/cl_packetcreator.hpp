#ifndef __CL_PACKETCREATOR_HPP__
#define __CL_PACKETCREATOR_HPP__

#include "common/Packet.hpp"

#include "cl_udppacket.hpp"
#include "tst_protocol.hpp"

namespace cl {

    class PacketCreator
    {
    public:
        static std::unique_ptr<Common::Packet> UdpReady(bool ready)
        {
            std::cout << "create UdpReady\n";
            Common::Packet* p = new Common::Packet();
            p->Write(tst_protocol::ClientToServer::clUdpReady);

            p->Write(ready);
            return std::unique_ptr<Common::Packet>(p);
        }

        static std::unique_ptr<UdpPacket> PassThrough(Uint32 id, Uint32 type)
        {
            std::cout << "create PassThrough (" << id << ") | " << type << " |\n";
            UdpPacket* p = new UdpPacket(id);
            p->Write(tst_protocol::ClientToServer::clPassThrough);

            p->Write(type);
            return std::unique_ptr<UdpPacket>(p);
        }

        static std::unique_ptr<Common::Packet> PassThroughOk(Uint32 type)
        {
            std::cout << "create PassThroughOk | " << type << " |\n";
            Common::Packet* p = new Common::Packet();
            p->Write(tst_protocol::ClientToServer::clPassThroughOk);

            p->Write(type);
            return std::unique_ptr<Common::Packet>(p);
        }
    };

}

#endif
