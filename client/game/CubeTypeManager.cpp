#include "client/precompiled.hpp"

#include "client/Client.hpp"
#include "client/game/CubeType.hpp"
#include "client/game/CubeTypeManager.hpp"
#include "client/network/PacketCreator.hpp"

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
        Tools::debug << "CubeType: " << type->name << " (id: " << type->id << ", description: " << type->luaFile << ")\n";
        this->_cubeTypes.push_back(CubeType(*type));
        this->_AskOneType();
    }

    void CubeTypeManager::LoadResources()
    {
        for (auto it = this->_cubeTypes.begin(), ite = this->_cubeTypes.end(); it != ite; ++it)
            it->Load(this->_client.GetGame().GetResourceManager());
    }

    void CubeTypeManager::_AskOneType()
    {
        assert(this->_nbCubeTypes != 0);
        if (this->_curAskedType <= this->_nbCubeTypes)
            this->_client.GetNetwork().SendPacket(Network::PacketCreator::GetCubeType(this->_curAskedType++));
    }

}}
