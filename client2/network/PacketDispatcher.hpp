#ifndef __CLIENT_NETWORK_PACKETDISPATCHER_HPP__
#define __CLIENT_NETWORK_PACKETDISPATCHER_HPP__

#include "protocol/protocol.hpp"

namespace Common {
    class Packet;
}
namespace Client {
    class Client;
}

namespace Client { namespace Network {

    class PacketDispatcher :
        private boost::noncopyable
    {
    private:
        std::function<void(Common::Packet&)> _dispatcher[(int)Protocol::ServerToClient::NbPacketTypeServer];
        Client& _client;

    public:
        PacketDispatcher(Client& client);
        void ProcessAllPackets(std::list<Common::Packet*>&& packets);

    private:
        void _ProcessPacket(Common::Packet& packet);
    };

}}

#endif
