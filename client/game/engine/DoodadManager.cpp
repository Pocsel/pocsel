#include "client/game/engine/DoodadManager.hpp"
#include "client/game/engine/Doodad.hpp"
#include "client/game/engine/DoodadType.hpp"
#include "client/game/engine/Engine.hpp"
#include "tools/lua/Interpreter.hpp"
#include "common/FieldValidator.hpp"

namespace Client { namespace Game { namespace Engine {

    DoodadManager::DoodadManager(Engine& engine) :
        _engine(engine), _runningDoodadId(0), _runningDoodad(0)
    {
    }

    Uint32 DoodadManager::GetRunningPluginId() const
    {
        return this->_runningDoodad ? this->_runningDoodad->GetType().GetPluginId() : 0;
    }

    Doodad const& DoodadManager::GetDoodad(Uint32 doodadId) const throw(std::runtime_error)
    {
        throw std::runtime_error("bite");
        //auto it = this->_doodads.find(doodadId);
        //if (it == this->_doodads.end() || !it->second)
        //    throw std::runtime_error("DoodadManager: Doodad not found.");
        //return *it->second;
    }

    void DoodadManager::SpawnDoodad(Uint32 doodadId,
            Uint32 pluginId,
            std::string const& doodadName,
            Common::Position const& position,
            std::list<std::pair<std::string /* key */, std::string /* value */>> const& values)
    {
    }

    void DoodadManager::KillDoodad(Uint32 doodadId)
    {
    }

    void DoodadManager::UpdateDoodad(Uint32 doodadId,
            Common::Position const* position,
            std::list<std::tuple<bool /* functionCall */, std::string /* function || key */, std::string /* value */>> const& commands)
    {
    }

    void DoodadManager::_ApiRegister(Tools::Lua::CallHelper& helper)
    {
        Uint32 pluginId = this->_engine.GetRunningPluginId();
        if (!pluginId)
            throw std::runtime_error("Client.Doodad.Register: Could not determine currently running plugin, aborting registration");
        Tools::Lua::Ref prototype(this->_engine.GetInterpreter().GetState());
        std::string doodadName;
        try
        {
            prototype = helper.PopArg("Client.Doodad.Register: Missing argument \"prototype\"");
            if (!prototype.IsTable())
                throw std::runtime_error("Client.Doodad.Register: Argument \"prototype\" must be a table");
            if (!prototype["doodadName"].IsString())
                throw std::runtime_error("Client.Doodad.Register: Field \"doodadName\" must exist and be a string");
            if (!Common::FieldValidator::IsRegistrableType(doodadName = prototype["doodadName"].ToString()))
                throw std::runtime_error("Client.Doodad.Register: Invalid doodad name \"" + doodadName + "\"");
            if (this->_doodadTypes[pluginId].count(doodadName))
                throw std::runtime_error("Client.Doodad.Register: A doodad with the name \"" + doodadName + "\" is already registered");
        }
        catch (std::exception& e)
        {
            Tools::error << "DoodadManager::_ApiRegister: Failed to register new doodad type from plugin " << pluginId << ": " << e.what() << std::endl;
            return;
        }
        this->_doodadTypes[pluginId][doodadName] = new DoodadType(pluginId, doodadName, prototype);
        Tools::debug << "Doodad \"" << doodadName << "\" registered (plugin: " << pluginId << ")." << std::endl;
    }

}}}
