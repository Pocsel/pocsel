#include "tools/VectorSerializer.hpp"

#include "protocol/protocol.hpp"

#include "common/Packet.hpp"
#include "common/Resource.hpp"
#include "common/ChunkSerializer.hpp"
#include "common/CubeTypeSerializer.hpp"
#include "common/PositionSerializer.hpp"

#include "server/network/PacketCreator.hpp"
#include "server/network/ChunkSerializer.hpp"

namespace Server { namespace Network {

    std::unique_ptr<Common::Packet> PacketCreator::LoggedIn(bool success,
                                            std::string const& reason /* = "" */,
                                            std::string const& worldIdentifier /* = "" */,
                                            std::string const& worldName /* = "" */,
                                            Uint32 worldVersion /* = 0 */,
                                            Chunk::CubeType nbCubeTypes /* = 0 */)
    {
        Common::Packet* p(new Common::Packet);
        p->Write(Protocol::ServerToClient::LoggedIn);
        p->WriteBool(success);
        p->Write(Protocol::Version::Major);
        p->Write(Protocol::Version::Minor);
        if (success)
        {
            p->Write(worldIdentifier);
            p->Write(worldName);
            p->Write(worldVersion);
            p->Write(nbCubeTypes);
        }
        else
        {
            assert(reason.size() > 0 && "Need a reason !");
            p->WriteString(reason);
        }
        return std::unique_ptr<Common::Packet>(p);
    }

    std::unique_ptr<Common::Packet> PacketCreator::Ping(Uint64 timestamp)
    {
        Common::Packet* p(new Common::Packet);
        p->Write((Protocol::ActionType)Protocol::ServerToClient::Ping);

        p->Write(timestamp); // timestamp agréé par le commité du temps
        return std::unique_ptr<Common::Packet>(p);
    }

    std::unique_ptr<Common::Packet> PacketCreator::Chunk(::Server::Chunk const& chunk)
    {
        Common::Packet* p(new Common::Packet);
        p->Write(Protocol::ServerToClient::Chunk);
        p->Write(chunk);
        return std::unique_ptr<Common::Packet>(p);
    }

    std::unique_ptr<Common::Packet> PacketCreator::NeededResourceIds(std::vector<Uint32>& ids,
                                                     Uint32& offset)
    {
        Common::Packet* response(new Common::Packet);
        response->Write(Protocol::ServerToClient::NeededResourceIds);
        response->Write32(static_cast<Uint32>(ids.size()));
        while (offset < ids.size() && offset < 15000)
        {
            response->Write(ids[offset]);
            ++offset;
        }
        return std::unique_ptr<Common::Packet>(response);
    }

    std::unique_ptr<Common::Packet> PacketCreator::ResourceRange(Common::Resource const& resource,
                                                 Uint32 offset)
    {
        std::unique_ptr<Common::Packet> ptr(new Common::Packet());
        ptr->Write(Protocol::ServerToClient::ResourceRange);
        ptr->Write32(resource.id);
        ptr->Write32(offset);
        Tools::debug << "PacketCreator::ResourceRange(): " << resource.id <<
                     ": offset = " << offset <<
                     " size = " << resource.size << ".\n";
        // XXX si packet change on est dans la merde !
        Uint32 packetSize = ((Uint16) -1) - sizeof(Protocol::ActionType) - 2 * sizeof(Uint32);
        if (offset == 0)
        {
            ptr->Write(resource.pluginId);
            ptr->Write(resource.type);
            ptr->Write(resource.filename);
            ptr->Write(resource.size);
            packetSize -= 2 * sizeof(Uint32) - 2 * sizeof(Uint16);
            packetSize -= static_cast<Uint32>(resource.filename.size() - resource.type.size()); // boarg
            if (packetSize >= ((Uint16) -1))
                throw std::runtime_error("overflow");
        }
        if (offset && offset >= resource.size)
            throw std::runtime_error("Invalid range offset");

        Uint32 toSendSize = std::min(resource.size, packetSize);
        if (toSendSize > resource.size - offset) // WARNING: this form prevents overflow
            throw std::runtime_error("Invalid range offset");
        ptr->WriteRawData(
                          ((char const*) resource.data) + offset,
                          toSendSize
                         );
        return ptr;
    }

    std::unique_ptr<Common::Packet> PacketCreator::CubeType(Common::CubeType const& cubeType)
    {
        Common::Packet* response(new Common::Packet);
        response->Write(Protocol::ServerToClient::CubeType);
        response->Write(cubeType);
        return std::unique_ptr<Common::Packet>(response);
    }

    std::unique_ptr<Common::Packet> PacketCreator::TeleportPlayer(std::string const& map,
                                                                  Common::Position const& pos)
    {
        Common::Packet* ptr(new Common::Packet);
        ptr->Write(Protocol::ServerToClient::TeleportPlayer);
        ptr->Write(map);
        ptr->Write(pos);
        return std::unique_ptr<Common::Packet>(ptr);
    }

}}