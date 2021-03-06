#include "client/precompiled.hpp"

#include <luasel/Luasel.hpp>

#include "client/resources/ResourceManager.hpp"
#include "client/game/engine/Engine.hpp"
#include "client/game/Game.hpp"
#include "client/game/engine/Doodad.hpp"
#include "client/game/engine/ModelManager.hpp"
#include "client/game/engine/SoundManager.hpp"
#include "client/game/engine/BodyManager.hpp"
#include "client/game/engine/PhysicsManager.hpp"
#include "tools/lua/utils/Utils.hpp"
#include "tools/gfx/utils/material/LuaMaterial.hpp"
#include "tools/gfx/utils/material/Material.hpp"
#include "common/FieldUtils.hpp"
#include "common/Resource.hpp"

namespace Client { namespace Game { namespace Engine {

    Engine::Engine(Game& game, Uint32 nbBodyTypes) :
        _game(game),
        _overriddenDoodadId(0)
    {
        this->_interpreter = new Luasel::Interpreter();

        // enregistrement des fonctions de base
        this->_interpreter->RegisterLib(Luasel::Interpreter::Base);
        this->_interpreter->Globals().Set("print", this->_interpreter->MakeFunction(std::bind(&Engine::_ApiPrint, this, std::placeholders::_1)));
        this->_interpreter->RegisterLib(Luasel::Interpreter::Math);
        this->_interpreter->RegisterLib(Luasel::Interpreter::Table);
        this->_interpreter->RegisterLib(Luasel::Interpreter::String);
        Tools::Gfx::Utils::Material::Material::LoadLuaTypes(*this->_interpreter,
            std::bind(static_cast<std::unique_ptr<Tools::Gfx::Utils::Texture::ITexture>(Resources::ResourceManager::*)(std::string const&)>(&Resources::ResourceManager::GetTexture), &game.GetResourceManager(), std::placeholders::_1),
            std::bind(static_cast<std::unique_ptr<Tools::Gfx::Utils::Material::LuaMaterial>(Resources::ResourceManager::*)(std::string const&)>(&Resources::ResourceManager::GetMaterial), &game.GetResourceManager(), std::placeholders::_1));
        Tools::Lua::Utils::RegisterVector(*this->_interpreter);
        Tools::Lua::Utils::RegisterMatrix(*this->_interpreter);

        // création des managers
        this->_doodadManager = new DoodadManager(*this);
        this->_modelManager = new ModelManager(*this);
        this->_soundManager = new SoundManager(*this);
        this->_bodyManager = new BodyManager(*this, nbBodyTypes);
        this->_physicsManager = new PhysicsManager(*this, this->_doodadManager->GetDoodads());
    }

    Engine::~Engine()
    {
        Tools::Delete(this->_modelManager);
        Tools::Delete(this->_soundManager);
        Tools::Delete(this->_doodadManager);
        Tools::Delete(this->_physicsManager);
        Tools::Delete(this->_bodyManager);
        Tools::Delete(this->_interpreter);
    }

    void Engine::Tick(Uint64 totalTime)
    {
        this->_physicsManager->Tick(totalTime);
        this->_doodadManager->Tick(totalTime);
        this->_modelManager->Tick(totalTime);
    }

    void Engine::LoadLuaScripts()
    {
        // enregistrement des fonctions spécifiques à chaque registrable client (Client.Effect.Register, Client.CubeMaterial.Register, ...)
        this->_game.GetResourceManager().RegisterLuaFunctions();
        this->_game.GetCubeTypeManager().RegisterLuaFunctions();

        // éxécution de tous les scripts
        this->_interpreter->Globals().Set("require", this->_interpreter->MakeFunction(std::bind(&Engine::_ApiRequire, this, std::placeholders::_1)));
        auto const& files = this->_game.GetResourceManager().GetDatabase().GetAllResources("lua");
        auto it = files.begin();
        auto itEnd = files.end();
        for (; it != itEnd; ++it)
            try
            {
                this->_LoadLuaScript((*it)->name);
            }
            catch (std::exception& e)
            {
                Tools::error << "Engine::LoadLuaScripts: " << e.what() << std::endl;
            }
        this->_interpreter->Globals().Set("require", this->_interpreter->MakeNil());
        this->_modules.clear(); // ne garde pas de references inutiles vers les modules
    }

    Luasel::Ref Engine::_LoadLuaScript(std::string const& name)
    {
        // check si le fichier a déjà été chargé
        auto itModule = this->_modules.find(name);
        if (itModule != this->_modules.end())
        {
            if (itModule->second.first) // loading in progress
                Tools::error << "Engine::_LoadLuaScript: Warning: Recursive require() of \"" << name << "\" (returning nil)." << std::endl;
            return itModule->second.second;
        }

        // le fichier n'est pas déjà chargé :
        std::string previousRunningResourceName = this->_runningResourceName;
        std::string previousRunningPluginName = this->_runningPluginName;
        this->_SetRunningResource(name);
        auto it = this->_modules.insert(std::make_pair(name, std::make_pair(true /* loading in progress */, this->_interpreter->MakeNil())));
        Luasel::Ref f = this->_interpreter->MakeNil();
        Luasel::Ref module = this->_interpreter->MakeNil();
        try
        {
            f = this->_interpreter->LoadString(this->_game.GetResourceManager().GetScript(name));
            module = f();
            if (module == f) // le module se retourne lui-même = pas de return dans le module = on retourne nil
                module = this->_interpreter->MakeNil();
            it.first->second.second = module;
            it.first->second.first = false; // loading not in progress anymore
            this->_SetRunningResource(Common::FieldUtils::GetResourceName(previousRunningPluginName, previousRunningResourceName));
            return module;
        }
        catch (std::exception& e)
        {
            this->_SetRunningResource(Common::FieldUtils::GetResourceName(previousRunningResourceName, previousRunningPluginName));
            it.first->second.first = false; // loading not in progress anymore
            throw std::runtime_error("Error while processing script resource \"" + name + "\" : " + e.what());
        }
    }

    void Engine::_ApiRequire(Luasel::CallHelper& helper)
    {
        std::string name = helper.PopArg("require: Missing argument \"name\"").CheckString("require: Argument \"name\" must be a string");
        helper.PushRet(this->_LoadLuaScript(name));
    }

    void Engine::_ApiPrint(Luasel::CallHelper& helper)
    {
        std::string str = "[";
        if (this->GetRunningPluginName() != "")
            str += this->GetRunningPluginName();
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
        str += "] ";
        auto it = helper.GetArgList().begin();
        auto itEnd = helper.GetArgList().end();
        for (; it != itEnd; ++it)
            str += it->ToString();
        Tools::log << str << std::endl;
    }

    void Engine::_SetRunningResource(std::string const& name)
    {
        this->_runningPluginName = Common::FieldUtils::GetPluginNameFromResource(name);
        this->_runningResourceName = Common::FieldUtils::GetResourceNameFromResource(name);
    }

}}}
