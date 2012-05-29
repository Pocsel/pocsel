#include "server/game/engine/BodyManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/Body.hpp"
#include "server/game/engine/BodyType.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/MetaTable.hpp"
#include "common/FieldValidator.hpp"

namespace Server { namespace Game { namespace Engine {

    BodyManager::BodyManager(Engine& engine) :
        _engine(engine)
    {
        Tools::debug << "BodyManager::BodyManager()\n";
        auto& i = this->_engine.GetInterpreter();
        auto namespaceTable = i.Globals()["Server"].Set("Body", i.MakeTable());
        namespaceTable.Set("Register", i.MakeFunction(std::bind(&BodyManager::_ApiRegister, this, std::placeholders::_1)));
    }

    BodyManager::~BodyManager()
    {
        Tools::debug << "BodyManager::~BodyManager()\n";
        // Body types
        auto itPlugin = this->_bodyTypes.begin();
        auto itPluginEnd = this->_bodyTypes.end();
        for (; itPlugin != itPluginEnd; ++itPlugin)
        {
            auto itType = itPlugin->second.begin();
            auto itTypeEnd = itPlugin->second.end();
            for (; itType != itTypeEnd; ++itType)
                Tools::Delete(itType->second);
        }
    }

    Body* BodyManager::_CreateBody(Uint32 pluginId, std::string bodyName) throw(std::runtime_error)
    {
        // trouve le plugin
        auto itPlugin = this->_bodyTypes.find(pluginId);
        if (itPlugin == this->_bodyTypes.end())
            throw std::runtime_error("unknown plugin " + Tools::ToString(pluginId));

        // trouve le prototype
        auto itType = itPlugin->second.find(bodyName);
        if (itType == itPlugin->second.end())
            throw std::runtime_error("unknown Body \"" + bodyName + "\" in plugin " + Tools::ToString(pluginId));

        // allocation
        Body* body;
        body = new Body(this->_engine.GetInterpreter(), *itType->second);

        Tools::debug << "BodyManager::_CreateBody: New Body \"" << bodyName << "\" (plugin " << pluginId << ") created.\n";
        return body;
    }

    void BodyManager::_ApiRegister(Tools::Lua::CallHelper& helper)
    {
        Uint32 pluginId = this->_engine.GetRunningPluginId();
        if (!pluginId)
            throw std::runtime_error("Server.Body.Register: Could not determine currently running plugin, aborting registration.");
        std::string pluginName = this->_engine.GetWorld().GetPluginManager().GetPluginIdentifier(pluginId);
        Tools::Lua::Ref prototype(this->_engine.GetInterpreter().GetState());
        std::string bodyName;
        try
        {
            prototype = helper.PopArg();
            if (!prototype.IsTable())
                throw std::runtime_error("Server.Body.Register[Positional]: Argument \"prototype\" must be of type table (instead of " + prototype.GetTypeName() + ")");
            if (!prototype["BodyName"].IsString())
                throw std::runtime_error("Server.Body.Register[Positional]: Field \"BodyName\" in prototype must exist and be of type string");
            if (!Common::FieldValidator::IsRegistrableType(bodyName = prototype["BodyName"].ToString()))
                throw std::runtime_error("Server.Body.Register[Positional]: Invalid Body name \"" + bodyName + "\"");
//            if (helper.GetNbArgs() && helper.PopArg().ToBoolean()) // bool flag en deuxieme parametre pour indiquer que c'est RegisterPositional()
//                positional = true;
        }
        catch (std::exception& e)
        {
            Tools::error << "BodyManager::_ApiRegister: Failed to register new Body type from \"" << pluginName << "\": " << e.what() << " (plugin " << pluginId << ").\n";
            return;
        }
        if (this->_bodyTypes[pluginId].count(bodyName)) // remplacement
        {
            BodyType* type = this->_bodyTypes[pluginId][bodyName];
            type->SetPrototype(prototype);
            Tools::log << "BodyManager::_ApiRegister: Replacing Body type \"" << bodyName << "\" with a newer type from \"" << pluginName << "\" (plugin " << pluginId << ").\n";
        }
        else // nouveau type
        {
            this->_bodyTypes[pluginId][bodyName] = new BodyType(bodyName, pluginId, prototype);
            Tools::debug << "BodyManager::_ApiRegister: New Body type \"" << bodyName << "\" registered from \"" << pluginName << "\" (plugin " << pluginId << ").\n";
        }
    }

}}}

