#ifndef __SV_PACKETCREATOR_HPP__
#define __SV_PACKETCREATOR_HPP__

#include "common/Packet.hpp"

#include "tst_protocol.hpp"
#include "sv_udppacket.hpp"

namespace sv {

    class PacketCreator
    {
    public:
        static std::unique_ptr<Common::Packet> Login(Uint32 id)
        {
            std::cout << "create Login | " << id << " |\n";
            Common::Packet* p = new Common::Packet();
            p->Write(tst_protocol::ServerToClient::svLogin);

            p->Write(id);
            return std::unique_ptr<Common::Packet>(p);
        }

        static std::unique_ptr<UdpPacket> PassThrough(Uint32 type)
        {
            std::cout << "create PassThrough | " << type << " |\n";
            UdpPacket* p = new UdpPacket(true);
            p->Write(tst_protocol::ServerToClient::svPassThrough);

            p->Write(type);
            return std::unique_ptr<UdpPacket>(p);
        }

        static std::unique_ptr<Common::Packet> PassThroughOk(Uint32 type)
        {
            std::cout << "create PassThroughOk | " << type << " |\n";
            Common::Packet* p = new Common::Packet();
            p->Write(tst_protocol::ServerToClient::svPassThroughOk);

            p->Write(type);
            return std::unique_ptr<Common::Packet>(p);
        }
    };

}

#endif
