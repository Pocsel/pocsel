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

    class Engine :
        private boost::noncopyable
    {
    private:
        Game& _game;
        Tools::Lua::Interpreter* _interpreter;
        DoodadManager* _doodadManager;
        Uint32 _overriddenPluginId;
        Uint32 _overriddenDoodadId;

    public:
        Engine(Game& game);
        ~Engine();
        void LoadLuaScripts();
        Game& GetGame() { return this->_game; }
        DoodadManager& GetDoodadManager() { return *this->_doodadManager; }
        Uint32 GetRunningPluginId() { return this->_overriddenPluginId ? this->_overriddenPluginId : this->_doodadManager->GetRunningPluginId(); }
        void OverrideRunningPluginId(Uint32 pluginId) { this->_overriddenPluginId = pluginId; }
        Uint32 GetRunningDoodadId() { return this->_overriddenDoodadId ? this->_overriddenDoodadId : this->_doodadManager->GetRunningDoodadId(); }
        void OverrideRunningDoodadId(Uint32 doodadId) { this->_overriddenDoodadId = doodadId; }
        Tools::Lua::Interpreter& GetInterpreter() { return *this->_interpreter; }
    private:
        void _ApiPrint(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
