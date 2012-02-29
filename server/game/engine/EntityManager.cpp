#include "server/game/engine/EntityManager.hpp"

namespace Server { namespace Game { namespace Engine {

    EntityManager::EntityManager(Engine& engine) :
        _engine(engine)
    {
        Tools::debug << "EntityManager::EntityManager()\n";
    }

    EntityManager::~EntityManager()
    {
        Tools::debug << "EntityManager::~EntityManager()\n";
    }

    void EntityManager::CallEntityFunction(int entityId, std::string function, Tools::Lua::Ref const& args)
    {
    }

}}}
