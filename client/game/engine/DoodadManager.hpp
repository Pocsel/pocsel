#ifndef __CLIENT_GAME_ENGINE_DOODADMANAGER_HPP__
#define __CLIENT_GAME_ENGINE_DOODADMANAGER_HPP__

namespace Tools { namespace Lua {
    class CallHelper;
}}

namespace Common { namespace Physics {
    struct Node;
    class World;
}}

namespace Client { namespace Game {
    class ShapeRenderer;
}}

namespace Client { namespace Game { namespace Engine {

    class Engine;
    class Doodad;
    class DoodadType;
    class Entity;

    class DoodadManager :
        private boost::noncopyable
    {
    private:
        Engine& _engine;
        std::map<std::string /* doodadName */, DoodadType*> _doodadTypes;
        std::map<Uint32 /* doodadId */, Doodad*> _doodads;
        std::map<Uint32 /* entityId */, Entity*> _entities;
        Uint32 _runningDoodadId; // 0 quand aucun doodad n'est en cours d'éxécution
        Doodad* _runningDoodad; // nul quand aucun doodad n'est en cours d'éxécution
        Uint64 _lastTime;
        ShapeRenderer* _shapeRenderer;

    public:
        DoodadManager(Engine& engine);
        ~DoodadManager();
        Uint32 GetRunningDoodadId() const { return this->_runningDoodadId; }
        Doodad* GetRunningDoodad() const { return this->_runningDoodad; }
        Doodad const& GetDoodad(Uint32 doodadId) const throw(std::runtime_error); // ne pas garder la référence, le doodad peut etre delete à tout moment
        std::map<Uint32, Doodad*> const& GetDoodads() const { return this->_doodads; }
        void Tick(Uint64 totalTime);
        void SpawnDoodad(Uint32 doodadId,
                Uint32 entityId,
                Uint32 bodyId,
                std::string const& doodadName,
                Common::Physics::Node const& position,
                std::list<std::pair<std::string /* key */, std::string /* value */>> const& values);
        void KillDoodad(Uint32 doodadId);
        void UpdateDoodad(Uint32 doodadId,
                std::vector<std::pair<bool, Common::Physics::Node>> const* body,
                std::list<std::tuple<bool /* functionCall */, std::string /* function || key */, std::string /* value */>> const& commands);
        void UpdateEntity(Uint32 entityId, Common::Physics::Node const& node);
        void Render();

    private:
        void _CallDoodadFunction(Uint32 doodadId, std::string const& function);
        void _ApiRegister(Tools::Lua::CallHelper& helper);
        void _UpdateEntityPosition(Entity& entity, Common::Physics::Node const& node);
    };

}}}

#endif
