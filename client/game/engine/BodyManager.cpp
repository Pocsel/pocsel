#include "client/game/engine/BodyManager.hpp"
#include "client/game/engine/Engine.hpp"

#include "client/game/Game.hpp"
#include "client/Client.hpp"
#include "client/network/Network.hpp"
#include "client/network/PacketCreator.hpp"

namespace Client { namespace Game { namespace Engine {

    BodyManager::BodyManager(Engine& engine, Uint32 nbBodyTypes) :
        _engine(engine),
        _nbBodyTypes(nbBodyTypes)
    {
        _bodyTypes.reserve(nbBodyTypes);
        this->_AskOneType();
    }

    void BodyManager::AddBodyType(std::unique_ptr<BodyType> bodyType)
    {
        Tools::debug << "new bodytype in bodymanager\n";
        this->_bodyTypes.emplace_back(*bodyType);
        this->_AskOneType();
    }

    void BodyManager::_AskOneType()
    {
        if (this->_nbBodyTypes == this->_bodyTypes.size())
            return;
        this->_engine.GetGame().GetClient().GetNetwork().SendPacket(Network::PacketCreator::GetBodyType((Uint32)this->_bodyTypes.size() + 1));
    }

}}}
