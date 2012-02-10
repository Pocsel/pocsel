#include <iostream>

#include "common/Packet.hpp"
#include "client2/network/PacketDispatcher.hpp"
#include "client2/Client.hpp"

namespace Client { namespace Network {

    PacketDispatcher::PacketDispatcher(Client& client)
        : _client(client)
    {
        this->_dispatcher[Protocol::ServerToClient::LoggedIn] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented LoggedIn\n"; };
        this->_dispatcher[Protocol::ServerToClient::Ping] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented Ping\n"; };
        this->_dispatcher[Protocol::ServerToClient::Chunk] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented Chunk\n"; };
        this->_dispatcher[Protocol::ServerToClient::NeededResourceIds] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented NeededResourceIds\n"; };
        this->_dispatcher[Protocol::ServerToClient::ResourceRange] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented ResourceRange\n"; };
        this->_dispatcher[Protocol::ServerToClient::CubeType] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented CubeType\n"; };
        this->_dispatcher[Protocol::ServerToClient::SpawnPosition] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented SpawnPosition\n"; };
    }

    void PacketDispatcher::ProcessAllPackets(std::list<Common::Packet*>&& packets)
    {
        for (auto it = packets.begin(), ite = packets.end(); it != ite; ++it)
        {
            this->_ProcessPacket(**it);
            delete *it;
        }
    }

    void PacketDispatcher::_ProcessPacket(Common::Packet& packet)
    {
        Protocol::ServerToClient id;
        packet.Read<Protocol::ServerToClient>(id);
        this->_dispatcher[id](packet);
    }

}}
