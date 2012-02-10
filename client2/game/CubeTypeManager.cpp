#include "client2/Client.hpp"
#include "client2/game/CubeTypeManager.hpp"
#include "client2/network/PacketCreator.hpp"

#include "common/Packet.hpp"

namespace Client { namespace Game {

    CubeTypeManager::CubeTypeManager(Client& client, Common::BaseChunk::CubeType nbCubeTypes)
        : _nbCubeTypes(nbCubeTypes),
        _client(client)
    {
        Tools::debug << "nbCubeTypes=" << this->_nbCubeTypes << "\n";
        this->_curAskedType = 1;
        this->_nbCubeTypes = nbCubeTypes;
        this->_AskOneType();
    }

    void CubeTypeManager::AddCubeType(std::unique_ptr<Common::CubeType> type)
    {
        Tools::debug
            << "CubeType: " << type->name << " (" << type->id << ")\n"
            << type->textures.top << "\n"
            << type->textures.left << "\n"
            << type->textures.front << "\n"
            << type->textures.right << "\n"
            << type->textures.back << "\n"
            << type->textures.bottom << "\n";
        this->_cubeTypes.push_back(Common::CubeType(*type));
        this->_AskOneType();
    }

    void CubeTypeManager::_AskOneType()
    {
        assert(this->_nbCubeTypes != 0);
        if (this->_curAskedType <= this->_nbCubeTypes)
            this->_client.GetNetwork().SendPacket(Network::PacketCreator::GetCubeType(this->_curAskedType++));
    }

}}
