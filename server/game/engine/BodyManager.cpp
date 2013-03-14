#include <luasel/Luasel.hpp>

#include "server/game/engine/BodyManager.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/Body.hpp"
#include "server/game/engine/BodyType.hpp"
#include "server/game/World.hpp"
#include "server/game/PluginManager.hpp"
#include "common/FieldUtils.hpp"

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

    Body* BodyManager::CreateBody(Uint32 pluginId, std::string bodyName, Common::Physics::BodyCluster& parent) throw(std::runtime_error)
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
        body = new Body(parent, *itType->second);

        Tools::debug << "BodyManager::_CreateBody: New Body \"" << bodyName << "\" (plugin " << pluginId << ") created.\n";
        return body;
    }

    void BodyManager::_ApiRegister(Luasel::CallHelper& helper)
    {
        Uint32 pluginId = this->_engine.GetCurrentPluginRegistering();
        if (!pluginId)
            throw std::runtime_error("Server.Body.Register: Could not determine currently running plugin, aborting registration.");
        std::string pluginName = this->_engine.GetWorld().GetPluginManager().GetPluginIdentifier(pluginId);
        Luasel::Ref prototype(this->_engine.GetInterpreter().GetState());
        std::string bodyName;
        try
        {
            prototype = helper.PopArg();
            if (!prototype.IsTable())
                throw std::runtime_error("Server.Body.Register[Positional]: Argument \"prototype\" must be of type table (instead of " + prototype.GetTypeName() + ")");
            if (!prototype["bodyName"].IsString())
                throw std::runtime_error("Server.Body.Register[Positional]: Field \"bodyName\" in prototype must exist and be of type string");
            if (!Common::FieldUtils::IsRegistrableType(bodyName = prototype["bodyName"].ToString()))
                throw std::runtime_error("Server.Body.Register[Positional]: Invalid Body name \"" + bodyName + "\"");
        }
        catch (std::exception& e)
        {
            Tools::error << "BodyManager::_ApiRegister: Failed to register new Body type from \"" << pluginName << "\": " << e.what() << " (plugin " << pluginId << ").\n";
            return;
        }
        if (this->_bodyTypes[pluginId].count(bodyName)) // remplacement
        {
            Tools::Delete(this->_bodyTypes[pluginId][bodyName]);
            Tools::log << "BodyManager::_ApiRegister: Replacing Body type \"" << bodyName << "\" with a newer type from \"" << pluginName << "\" (plugin " << pluginId << ").\n";
        }
        this->_bodyTypesVec.push_back(new BodyType(bodyName, pluginId, (Uint32)this->_bodyTypesVec.size() + 1, prototype));
        this->_bodyTypes[pluginId][bodyName] = this->_bodyTypesVec.back();
        Tools::debug << "BodyManager::_ApiRegister: New Body type \"" << bodyName << "\" registered from \"" << pluginName << "\" (plugin " << pluginId << ").\n";
    }

}}}

