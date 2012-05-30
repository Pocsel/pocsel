#ifndef __CLIENT_GAME_ENGINE_DOODADMANAGER_HPP__
#define __CLIENT_GAME_ENGINE_DOODADMANAGER_HPP__

#include "common/Position.hpp"

namespace Tools { namespace Lua {
    class CallHelper;
}}

namespace Client { namespace Game { namespace Engine {

    class Engine;
    class Doodad;
    class DoodadType;

    class DoodadManager :
        private boost::noncopyable
    {
    private:
        Engine& _engine;
        std::map<Uint32 /* pluginId */, std::map<std::string /* doodadName */, DoodadType*>> _doodadTypes;
        std::map<Uint32 /* doodadId */, Doodad*> _doodads;
        Uint32 _runningDoodadId; // 0 quand aucun doodad n'est en cours d'éxécution
        Doodad* _runningDoodad; // nul quand aucun doodad n'est en cours d'éxécution

    public:
        DoodadManager(Engine& engine);
        ~DoodadManager();
        Uint32 GetRunningDoodadId() const { return this->_runningDoodadId; }
        Doodad* GetRunningDoodad() const { return this->_runningDoodad; }
        Uint32 GetRunningPluginId() const;
        Doodad const& GetDoodad(Uint32 doodadId) const throw(std::runtime_error); // ne pas garder la référence, le doodad peut etre delete à tout moment
        void Tick();
        void SpawnDoodad(Uint32 doodadId,
                Uint32 pluginId,
                std::string const& doodadName,
                Common::Position const& position,
                std::list<std::pair<std::string /* key */, std::string /* value */>> const& values);
        void KillDoodad(Uint32 doodadId);
        void UpdateDoodad(Uint32 doodadId,
                Common::Position const* position,
                std::list<std::tuple<bool /* functionCall */, std::string /* function || key */, std::string /* value */>> const& commands);
    private:
        void _CallDoodadFunction(Uint32 doodadId, std::string const& function);
        void _ApiRegister(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
