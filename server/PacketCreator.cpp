
#include <iostream>
#include <stdexcept>

#include "tools/VectorSerializer.hpp"

#include "protocol/protocol.hpp"

#include "common/Packet.hpp"
#include "common/Resource.hpp"
#include "common/ChunkSerializer.hpp"
#include "common/CubeTypeSerializer.hpp"
#include "common/PositionSerializer.hpp"

#include "server/PacketCreator.hpp"
#include "server/ChunkSerializer.hpp"

using namespace Server;

Common::PacketPtr PacketCreator::LoggedInPacket(bool success,
        std::string const& reason /* = "" */,
        std::string const& worldIdentifier /* = "" */,
        std::string const& worldName /* = "" */,
        Uint32 worldVersion /* = 0 */,
        Chunk::CubeType nbCubeTypes /* = 0 */)
{
    Common::PacketPtr p(new Common::Packet);
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
    return p;
}

Common::PacketPtr PacketCreator::PingPacket()
{
    Common::PacketPtr p(new Common::Packet);
    p->Write((Protocol::ActionType)Protocol::ServerToClient::Ping);
    p->Write64(2134); // timestamp agréé par le commité du temps
    return p;
}

Common::PacketPtr PacketCreator::ChunkPacket(Chunk const& chunk)
{
    Common::PacketPtr p(new Common::Packet);
    p->Write(Protocol::ServerToClient::Chunk);
    p->Write(chunk);
    return p;
}

Common::PacketPtr PacketCreator::ResourceRangePacket(Common::Resource const& resource, Uint32 offset)
{
    Common::PacketPtr ptr(new Common::Packet());
    ptr->Write(Protocol::ServerToClient::ResourceRange);
    ptr->Write32(resource.id);
    ptr->Write32(offset);
    std::cout << "PacketCreator::ResourceRangePacket(): Sending resource " << resource.id << ": "
              << "offset = " << offset << " size = " << resource.size << ".\n";
    // XXX si packet change on est dans la merde !
    Uint32 packetSize = ((Uint16) -1) - sizeof(Protocol::ActionType) - 2 * sizeof(Uint32);
    if (offset == 0)
    {
        ptr->Write(resource.pluginId);
        ptr->Write(resource.type);
        ptr->Write(resource.filename);
        ptr->Write(resource.size);
        packetSize -= 2 * sizeof(Uint32) - 2 * sizeof(Uint16);
        packetSize -= resource.filename.size() - resource.type.size(); // boarg
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

Common::PacketPtr PacketCreator::CubeTypePacket(Common::CubeType const& cubeType)
{
    Common::PacketPtr response(new Common::Packet);
    response->Write(Protocol::ServerToClient::CubeType);
    response->Write(cubeType);
    return response;
}

Common::PacketPtr PacketCreator::NeededResourceIdsPacket(std::vector<Uint32>& ids, Uint32& offset)
{
    Common::PacketPtr response(new Common::Packet);
    response->Write(Protocol::ServerToClient::NeededResourceIds);
    response->Write32(ids.size());
    while (offset < ids.size() && offset < 15000)
    {
        response->Write(ids[offset]);
        ++offset;
    }
    return response;
}

Common::PacketPtr PacketCreator::SpawnPosition(Common::Position const& pos)
{
    Common::PacketPtr ptr(new Common::Packet);
    ptr->Write(Protocol::ServerToClient::SpawnPosition);
    ptr->Write(pos);
    return ptr;
}
