#ifndef __CLIENT_NETWORK_PACKETDISPATCHER_HPP__
#define __CLIENT_NETWORK_PACKETDISPATCHER_HPP__

#include "protocol/protocol.hpp"

namespace Tools {
    class ByteArray;
}
namespace Client {
    class Client;
}

namespace Client { namespace Network {

    class PacketDispatcher :
        private boost::noncopyable
    {
    private:
        std::function<void(Tools::ByteArray&)> _dispatcher[(int)Protocol::ServerToClient::NbPacketTypeServer];
        Client& _client;

    public:
        PacketDispatcher(Client& client);
        void ProcessAllPackets(std::list<Tools::ByteArray*>&& packets);

    private:
        void _ProcessPacket(Tools::ByteArray& packet);
        void _HandleLogin(Tools::ByteArray& p);
        void _HandleNeededResourceIds(Tools::ByteArray& p);
    };

}}

#endif
