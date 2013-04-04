#ifndef __CLIENT_GAME_ENGINE_ENGINE_HPP__
#define __CLIENT_GAME_ENGINE_ENGINE_HPP__

#include "client/game/engine/DoodadManager.hpp"

namespace Luasel {
    class Interpreter;
    class CallHelper;
    class Ref;
}
namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Game { namespace Engine {

    class ModelManager;
    class BodyManager;
    class BodyType;
    class PhysicsManager;

    class Engine :
        private boost::noncopyable
    {
    private:
        Game& _game;
        Luasel::Interpreter* _interpreter;
        DoodadManager* _doodadManager;
        ModelManager* _modelManager;
        BodyManager* _bodyManager;
        PhysicsManager* _physicsManager;
        std::string _runningPluginName;
        std::string _runningResourceName;
        Uint32 _overriddenDoodadId;
        std::map<std::string /* script name */, std::pair<bool /* loading in progress */, Luasel::Ref /* module */>> _modules;

    public:
        Engine(Game& game, Uint32 nbBodyTypes);
        ~Engine();
        void Tick(Uint64 totalTime);
        void LoadLuaScripts();
        Game& GetGame() { return this->_game; }
        DoodadManager& GetDoodadManager() { return *this->_doodadManager; }
        DoodadManager const& GetDoodadManager() const { return *this->_doodadManager; }
        ModelManager& GetModelManager() { return *this->_modelManager; }
        ModelManager const& GetModelManager() const { return *this->_modelManager; }
        BodyManager& GetBodyManager() { return *this->_bodyManager; }
        BodyManager const& GetBodyManager() const { return *this->_bodyManager; }
        PhysicsManager& GetPhysicsManager() { return *this->_physicsManager; }
        PhysicsManager const& GetPhysicsManager() const { return *this->_physicsManager; }
        std::string const& GetRunningPluginName() const { return this->_runningPluginName; }
        std::string const& GetRunningResourceName() const { return this->_runningResourceName; }
        Uint32 GetRunningDoodadId() { return this->_overriddenDoodadId ? this->_overriddenDoodadId : this->_doodadManager->GetRunningDoodadId(); }
        void OverrideRunningDoodadId(Uint32 doodadId) { this->_overriddenDoodadId = doodadId; }
        Luasel::Interpreter& GetInterpreter() { return *this->_interpreter; }
    private:
        Luasel::Ref _LoadLuaScript(std::string const& name);
        void _ApiRequire(Luasel::CallHelper& helper);
        void _ApiPrint(Luasel::CallHelper& helper);
        void _SetRunningResource(std::string const& name);
    };

}}}

#endif
