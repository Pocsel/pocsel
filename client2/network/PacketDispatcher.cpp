#include "common/Packet.hpp"
#include "client2/network/Network.hpp"
#include "client2/network/PacketCreator.hpp"
#include "client2/network/PacketDispatcher.hpp"
#include "client2/network/PacketExtractor.hpp"
#include "client2/Client.hpp"

namespace Client { namespace Network {

    PacketDispatcher::PacketDispatcher(Client& client)
        : _client(client)
    {
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::LoggedIn] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented LoggedIn\n"; };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::Ping] =
            [this](Common::Packet&)
            {
                this->_client.GetNetwork().SendPacket(PacketCreator::Pong());
            };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::Chunk] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented Chunk\n"; };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::NeededResourceIds] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented NeededResourceIds\n"; };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::ResourceRange] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented ResourceRange\n"; };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::CubeType] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented CubeType\n"; };
        this->_dispatcher[(Protocol::ActionType)Protocol::ServerToClient::TeleportPlayer] = [](Common::Packet&) { Tools::debug << "PacketDispatcher: Not implemented SpawnPosition\n"; };
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
        Protocol::ActionType id;
        packet.Read(id);
        this->_dispatcher[id](packet);
    }

}}
