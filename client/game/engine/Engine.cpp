#include "client/game/engine/Engine.hpp"
#include "client/game/Game.hpp"
#include "client/game/engine/Doodad.hpp"
#include "tools/lua/Interpreter.hpp"
#include "tools/lua/utils/Utils.hpp"
#include "common/Resource.hpp"

namespace Client { namespace Game { namespace Engine {

    Engine::Engine(Game& game) :
        _game(game), _overriddenPluginId(0), _overriddenDoodadId(0)
    {
        this->_interpreter = new Tools::Lua::Interpreter();

        // enregistrement des fonctions de base
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Base);
        this->_interpreter->Globals().Set("print", this->_interpreter->MakeFunction(std::bind(&Engine::_ApiPrint, this, std::placeholders::_1)));
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Math);
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::Table);
        this->_interpreter->RegisterLib(Tools::Lua::Interpreter::String);
        Tools::Lua::Utils::RegisterColor(*this->_interpreter);

        // création des managers
        this->_doodadManager = new DoodadManager(*this);
    }

    Engine::~Engine()
    {
        Tools::Delete(this->_doodadManager);
        Tools::Delete(this->_interpreter);
    }

    void Engine::LoadLuaScripts()
    {
        // enregistrement des fonctions spécifiques à chaque registrable client (Client.Effect.Register, Client.CubeMaterial.Register, ...)
        this->_game.GetResourceManager().RegisterLuaFunctions();
        this->_game.GetCubeTypeManager().RegisterLuaFunctions();

        // éxécution de tous les scripts
        auto const& files = this->_game.GetResourceManager().GetDatabase().GetAllResources("lua");
        auto it = files.begin();
        auto itEnd = files.end();
        for (; it != itEnd; ++it)
        {
            this->OverrideRunningPluginId((*it)->pluginId);
            try
            {
                this->_interpreter->DoString(std::string(static_cast<char const*>((*it)->data), (*it)->size));
            }
            catch (std::exception& e)
            {
                Tools::error << "Error while processing script resource \"" << (*it)->name << "\" from plugin " << (*it)->pluginId << ": " << e.what() << std::endl;
            }
            this->OverrideRunningPluginId(0);
        }
    }

    void Engine::_ApiPrint(Tools::Lua::CallHelper& helper)
    {
        std::string str = "[";
        if (this->GetRunningPluginId())
            str += Tools::ToString(this->GetRunningPluginId());
        else
            str += "?";
        str += "/";
        try
        {
            auto& d = this->_doodadManager->GetDoodad(this->GetRunningDoodadId());
            str += d.GetType().GetName() + "/" + Tools::ToString(this->GetRunningDoodadId());
        }
        catch (std::exception&)
        {
            str += "?";
        }
        str += "]";
        auto it = helper.GetArgList().begin();
        auto itEnd = helper.GetArgList().end();
        for (; it != itEnd; ++it)
            str += it->ToString();
        Tools::log << str << std::endl;
    }

}}}