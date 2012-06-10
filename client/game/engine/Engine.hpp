#ifndef __CLIENT_GAME_ENGINE_ENGINE_HPP__
#define __CLIENT_GAME_ENGINE_ENGINE_HPP__

#include "client/game/engine/DoodadManager.hpp"

namespace Tools { namespace Lua {
    class Interpreter;
    class CallHelper;
}}
namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Game { namespace Engine {

    class ModelManager;

    class Engine :
        private boost::noncopyable
    {
    private:
        Game& _game;
        Tools::Lua::Interpreter* _interpreter;
        DoodadManager* _doodadManager;
        ModelManager* _modelManager;
        std::string _pluginName;
        std::string _resourceName;
        Uint32 _overriddenDoodadId;

    public:
        Engine(Game& game);
        ~Engine();
        void Tick(Uint64 totalTime);
        void LoadLuaScripts();
        Game& GetGame() { return this->_game; }
        DoodadManager& GetDoodadManager() { return *this->_doodadManager; }
        ModelManager& GetModelManager() { return *this->_modelManager; }
        std::string const& GetRunningPluginName() const { return this->_pluginName; }
        std::string const& GetRunningResourceName() const { return this->_resourceName; }
        Uint32 GetRunningDoodadId() { return this->_overriddenDoodadId ? this->_overriddenDoodadId : this->_doodadManager->GetRunningDoodadId(); }
        void OverrideRunningDoodadId(Uint32 doodadId) { this->_overriddenDoodadId = doodadId; }
        Tools::Lua::Interpreter& GetInterpreter() { return *this->_interpreter; }
    private:
        void _ApiPrint(Tools::Lua::CallHelper& helper);
        void _SetRunningResource(std::string const& name);
    };

}}}

#endif
