#include <iostream>

#include "client/CubeTypeManager.hpp"
#include "client/PacketDispatcher.hpp"
#include "client/PacketCreator.hpp"
#include "client/PacketExtractor.hpp"
#include "client/Network.hpp"

#include "tools/gui/EventManager.hpp"

#include "common/Packet.hpp"

namespace Client {

    CubeTypeManager::CubeTypeManager(Tools::Gui::EventManager& eventManager,
            Network& network,
            PacketDispatcher& packetDispatcher) :
        _network(network),
        _eventManager(eventManager),
        _nbCubeTypes(0)
    {
        packetDispatcher.Register(
                Protocol::ServerToClient::CubeType,
                std::bind(&CubeTypeManager::_HandleCubeType, this, std::placeholders::_1)
                );
    }

    void CubeTypeManager::Load(Chunk::CubeType nbCubeTypes)
    {
        std::cout << "nbCubeTypes=" << nbCubeTypes << "\n";
        assert(this->_nbCubeTypes == 0);
        this->_curAskedType = 1;
        this->_nbCubeTypes = nbCubeTypes;
        this->_AskOneType();
    }

    void CubeTypeManager::_HandleCubeType(Common::Packet const& p)
    {
        std::unique_ptr<Common::CubeType> type = PacketExtractor::ExtractCubeType(p);
        std::cout << type->id << std::endl;
        std::cout << type->name << std::endl;
        std::cout << type->textures.top << std::endl;
            std::cout << type->textures.left << std::endl;
            std::cout << type->textures.front << std::endl;
            std::cout << type->textures.right << std::endl;
            std::cout << type->textures.back << std::endl;
            std::cout << type->textures.bottom << std::endl;
        this->_cubeTypes.push_back(Common::CubeType(*type));
        this->_AskOneType();
    }

    void CubeTypeManager::_AskOneType()
    {
        assert(this->_nbCubeTypes != 0);
        if (this->_curAskedType <= this->_nbCubeTypes)
        {
            Common::PacketPtr p(new Common::Packet);
            this->_network.SendPacket(
                PacketCreator::GetCubeTypePacket(this->_curAskedType));
            ++this->_curAskedType;
        }
        else
        {
            this->_eventManager.Notify<Tools::Gui::Event>("cubetypes/loaded");
        }
    }

}
