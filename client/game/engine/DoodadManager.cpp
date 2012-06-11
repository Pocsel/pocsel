#include "client/game/engine/DoodadManager.hpp"
#include "client/game/engine/ModelManager.hpp"
#include "client/game/engine/Doodad.hpp"
#include "client/game/engine/DoodadType.hpp"
#include "client/game/engine/Engine.hpp"
#include "tools/lua/Interpreter.hpp"
#include "common/FieldUtils.hpp"

namespace Client { namespace Game { namespace Engine {

    DoodadManager::DoodadManager(Engine& engine) :
        _engine(engine), _runningDoodadId(0), _runningDoodad(0)
    {
        auto& i = this->_engine.GetInterpreter();
        auto namespaceTable = i.Globals().GetTable("Client").GetTable("Doodad");
        namespaceTable.Set("Register", i.MakeFunction(std::bind(&DoodadManager::_ApiRegister, this, std::placeholders::_1)));
    }

    DoodadManager::~DoodadManager()
    {
        // doodads
        auto itDoodad = this->_doodads.begin();
        auto itDoodadEnd = this->_doodads.end();
        for (; itDoodad != itDoodadEnd; ++itDoodad)
            Tools::Delete(itDoodad->second);
        // doodad types
        auto itPlugin = this->_doodadTypes.begin();
        auto itPluginEnd = this->_doodadTypes.end();
        for (; itPlugin != itPluginEnd; ++itPlugin)
            Tools::Delete(itPlugin->second);
    }

    //Uint32 DoodadManager::GetRunningPluginId() const
    //{
    //    return this->_runningDoodad ? this->_runningDoodad->GetType().GetPluginId() : 0;
    //}

    Doodad const& DoodadManager::GetDoodad(Uint32 doodadId) const throw(std::runtime_error)
    {
        auto it = this->_doodads.find(doodadId);
        if (it == this->_doodads.end() || !it->second)
            throw std::runtime_error("DoodadManager::GetDoodad: Doodad not found.");
        return *it->second;
    }

    void DoodadManager::Tick()
    {
        auto it = this->_doodads.begin();
        auto itEnd = this->_doodads.end();
        for (; it != itEnd; ++it)
            this->_CallDoodadFunction(it->first, "Think");
    }

    void DoodadManager::SpawnDoodad(Uint32 doodadId,
            std::string const& doodadName,
            Common::Position const& position,
            std::list<std::pair<std::string /* key */, std::string /* value */>> const& values)
    {
        if (this->_doodads.count(doodadId))
        {
            Tools::error << "DoodadManager::SpawnDoodad: Doodad " << doodadId << " already exists." << std::endl;
            return;
        }
        auto it = this->_doodadTypes.find(doodadName);
        if (it == this->_doodadTypes.end())
        {
            Tools::error << "DoodadManager::SpawnDoodad: Doodad type \"" << doodadName << "\" not found." << std::endl;
            return;
        }
        this->_doodads[doodadId] = new Doodad(this->_engine.GetInterpreter(), doodadId, position, *it->second);
        auto itValues = values.begin();
        auto itValuesEnd = values.end();
        for (; itValues != itValuesEnd; ++itValues)
        {
            auto const& v = *itValues;
            // TODO
        }
        this->_CallDoodadFunction(doodadId, "Spawn");
    }

    void DoodadManager::KillDoodad(Uint32 doodadId)
    {
        auto it = this->_doodads.find(doodadId);
        if (it != this->_doodads.end())
        {
            this->_CallDoodadFunction(doodadId, "Die");
            this->_engine.GetModelManager().DeleteModelsOfDoodad(doodadId);
            Tools::Delete(it->second);
            this->_doodads.erase(it);
        }
        else
            Tools::error << "DoodadManager::KillDoodad: Doodad " << doodadId << " does not exist." << std::endl;
    }

    void DoodadManager::UpdateDoodad(Uint32 doodadId,
            Common::Position const* position,
            std::list<std::tuple<bool /* functionCall */, std::string /* function || key */, std::string /* value */>> const& commands)
    {
        auto it = this->_doodads.find(doodadId);
        if (it == this->_doodads.end())
        {
            Tools::error << "DoodadManager::UpdateDoodad: Doodad " << doodadId << " not found." << std::endl;
            return;
        }
        if (position)
            it->second->SetPosition(*position);
        auto itCommands = commands.begin();
        auto itCommandsEnd = commands.end();
        for (; itCommands != itCommandsEnd; ++itCommands)
        {
            auto const& c = *itCommands;
            if (std::get<0>(c)) // functionCall
            {
                this->_CallDoodadFunction(doodadId, std::get<1>(c) /* TODO value */);
            }
            else // pas functionCall
            {
                // TODO
            }
        }
    }

    void DoodadManager::_CallDoodadFunction(Uint32 doodadId, std::string const& function)
    {
        assert(!this->_runningDoodadId && !this->_engine.GetRunningDoodadId() && "chainage de calls Lua, THIS IS BAD");
        auto it = this->_doodads.find(doodadId);
        if (it == this->_doodads.end())
        {
            Tools::error << "DoodadManager::_CallDoodadFunction: Call to \"" << function << "\" for doodad " << doodadId << " failed: doodad not found.\n";
            return;
        }
        try
        {
            auto prototype = it->second->GetType().GetPrototype();
            if (!prototype.IsTable())
                throw std::runtime_error("prototype is not a table"); // le moddeur a fait de la merde avec son type...
            auto f = it->second->GetType().GetPrototype()[function];
            if (f.IsFunction())
            {
                this->_runningDoodadId = doodadId;
                this->_runningDoodad = it->second;
                f();
                this->_runningDoodad = 0;
                this->_runningDoodadId = 0;
            }
            else
            {
                Tools::error << "DoodadManager::_CallDoodadFunction: Call to \"" << function << "\" for doodad " << doodadId << " (\"" << it->second->GetType().GetName() << "\") failed: function not found (type: " << f.GetTypeName() << ").\n";
            }
        }
        catch (std::exception& e)
        {
            Tools::error << "DoodadManager::_CallDoodadFunction: Fatal (doodad deleted): Call to \"" << function << "\" for doodad " << doodadId << " (\"" << it->second->GetType().GetName() << "\") failed: " << e.what() << std::endl;
            // TODO delete doodad
            this->_runningDoodad = 0;
            this->_runningDoodadId = 0;
            return;
        }
    }

    void DoodadManager::_ApiRegister(Tools::Lua::CallHelper& helper)
    {
        std::string const& pluginName = this->_engine.GetRunningPluginName();
        if (pluginName == "")
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
            if (!Common::FieldUtils::IsRegistrableType(doodadName = prototype["doodadName"].ToString()))
                throw std::runtime_error("Client.Doodad.Register: Invalid doodad name \"" + doodadName + "\"");
            doodadName = Common::FieldUtils::GetResourceName(pluginName, doodadName);
            if (this->_doodadTypes.count(doodadName))
                throw std::runtime_error("Client.Doodad.Register: A doodad with the name \"" + doodadName + "\" is already registered");
        }
        catch (std::exception& e)
        {
            Tools::error << "DoodadManager::_ApiRegister: Failed to register new doodad type from plugin " << pluginName << " (file: " << this->_engine.GetRunningResourceName() << "): " << e.what() << std::endl;
            return;
        }
        this->_doodadTypes[doodadName] = new DoodadType(doodadName, prototype);
        Tools::debug << "Doodad \"" << doodadName << "\" registered." << std::endl;
    }

}}}
