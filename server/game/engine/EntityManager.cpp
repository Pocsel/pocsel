#include "server/game/engine/EntityManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "tools/lua/Interpreter.hpp"

namespace Server { namespace Game { namespace Engine {

    EntityManager::EntityManager(Engine& engine) :
        _engine(engine), _pluginIdForRegistering(0), _currentEntityId(0)
    {
        Tools::debug << "EntityManager::EntityManager()\n";
        this->_engine.GetInterpreter().Globals()["Server"].Set("Entity", this->_engine.GetInterpreter().MakeTable());
    }

    EntityManager::~EntityManager()
    {
        Tools::debug << "EntityManager::~EntityManager()\n";
        std::for_each(this->_entities.begin(), this->_entities.end(), [](std::pair<int const, Entity*>& it) { Tools::Delete(it.second); });
        auto itPlugin = this->_entityTypes.begin();
        auto itPluginEnd = this->_entityTypes.end();
        for (; itPlugin != itPluginEnd; ++itPlugin)
        {
            auto itType = itPlugin->second.begin();
            auto itTypeEnd = itPlugin->second.end();
            for (; itType != itTypeEnd; ++itType)
                Tools::Delete(itType->second);
        }
    }

    void EntityManager::BeginPluginRegistering(Uint32 pluginId)
    {
        this->_pluginIdForRegistering = pluginId;
        this->_engine.GetInterpreter().Globals()["Server"]["Entity"].Set("Register",
                this->_engine.GetInterpreter().MakeFunction(std::bind(&EntityManager::_Register, this, std::placeholders::_1)));
    }

    void EntityManager::EndPluginRegistering()
    {
        this->_pluginIdForRegistering = 0;
        this->_engine.GetInterpreter().Globals()["Server"]["Entity"].Set("Register",
                this->_engine.GetInterpreter().MakeNil());
    }

    int EntityManager::SpawnEntity(std::string name, Uint32 pluginId, Tools::Lua::Ref const& args) throw(std::runtime_error)
    {
        auto itPlugin = this->_entityTypes.find(pluginId);
        if (itPlugin == this->_entityTypes.end())
            throw std::runtime_error("EntityManager::SpawnEntity: Unknown plugin " + Tools::ToString(pluginId));
        auto itType = itPlugin->second.find(name);
        if (itType == itPlugin->second.end())
            throw std::runtime_error("EntityManager::SpawnEntity: Unknown entity \"" + name + "\" in plugin " + Tools::ToString(pluginId));
        while (this->_entities.count(this->_currentEntityId))
            ++this->_currentEntityId;
        this->_entities[this->_currentEntityId] = new Entity(itType->second, this->_engine.GetInterpreter().MakeTable());
        Tools::debug << "EntityManager::SpawnEntity: New entity \"" << name << "\" (plugin " << pluginId << ") spawned.\n";
        this->CallEntityFunction(this->_currentEntityId, "Spawn", args);
        return this->_currentEntityId++;
    }

    void EntityManager::BootstrapPlugin(Uint32 pluginId)
    {
        // TODO rajouter un "if !(le plugin a deja ete charge une fois dans cette map)"
        try
        {
            this->SpawnEntity("Init", pluginId, this->_engine.GetInterpreter().MakeNil());
        }
        catch (std::exception& e)
        {
            Tools::log << "EntityManager::StartPlugin: Error: " << e.what() << std::endl;
        }
    }

    void EntityManager::CallEntityFunction(int entityId, std::string function, Tools::Lua::Ref const& args)
    {
        auto it = this->_entities.find(entityId);
        if (it == this->_entities.end())
        {
            Tools::error << "EntityManager::CallEntityFunction: Call to \"" << function << "\" for entity " << entityId << " failed: entity not found.\n";
            return;
        }
        try
        {
            it->second->self.Set("id", entityId);
            it->second->type->type[function](it->second->type->type, it->second->self, args);
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::CallEntityFunction: Call to \"" << function << "\" for entity " << entityId << " failed: " << e.what() << std::endl;
            return;
        }
        Tools::debug << "EntityManager::CallEntityFunction: Function \"" << function << "\" called for entity " << entityId << ".\n";
    }

    void EntityManager::_Register(Tools::Lua::CallHelper& helper)
    {
        assert(this->_pluginIdForRegistering != 0 && "_Register ne doit pas etre accessible en dehors de la phase d'enregistrement des entites");
        Tools::Lua::Ref t(this->_engine.GetInterpreter().GetState());
        try
        {
            t = helper.PopArg();
            if (!t.IsTable())
                throw std::runtime_error("expected one parameter of type table");
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::_Register: " << e.what() << std::endl;
            return;
        }
        std::string name;
        try
        {
            name = t["name"].CheckString();
            if (name.empty())
                throw std::runtime_error("name is empty");
        }
        catch (std::exception& e)
        {
            Tools::error << "EntityManager::_Register: Invalid entity name: " << e.what() << std::endl;
            return;
        }
        EntityType* type = new EntityType(name, this->_pluginIdForRegistering, t);
        this->_entityTypes[this->_pluginIdForRegistering][name] = type;
        Tools::debug << "EntityManager::_Register: New entity type \"" << type->name << "\" registered (plugin " << type->pluginId << ").\n";
    }

}}}
