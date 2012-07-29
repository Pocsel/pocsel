#ifndef __SERVER_GAME_ENGINE_DOODADMANAGER_HPP__
#define __SERVER_GAME_ENGINE_DOODADMANAGER_HPP__

namespace Tools {
    namespace Lua {
        class CallHelper;
    }
    namespace Database {
        class IConnection;
    }
}

namespace Server { namespace Game { namespace Engine {

    class Engine;
    class Doodad;
    class PositionalEntity;

    class DoodadManager :
        private boost::noncopyable
    {
    private:
        Engine& _engine;
        std::map<Uint32 /* doodadId */, Doodad*> _doodads; // pointeur nul quand un doodad est désactivé
        std::map<Uint32 /* entityId */, std::list<Doodad*>> _doodadsByEntity;
        std::map<Uint32 /* entityId */, std::list<Doodad*>> _disabledDoodads;
        std::unordered_set<Doodad*> _dirtyDoodads;
        Uint32 _nextDoodadId;

    public:
        DoodadManager(Engine& engine);
        ~DoodadManager();
        void Save(Tools::Database::IConnection& conn);
        void Load(Tools::Database::IConnection& conn);
        void ExecuteCommands();
        //void DoodadRemovedForPlayer(Uint32 playerId, Uint32 doodadId);
        void DeleteDoodadsOfEntity(Uint32 entityId);
        void DisableDoodadsOfEntity(Uint32 entityId);
        void EnableDoodadsOfEntity(Uint32 entityId);
        bool EntityHasDoodad(Uint32 entityId) const;
        //void EntityHasMoved(Uint32 entityId);
        void DoodadIsDirty(Doodad* doodad) { this->_dirtyDoodads.insert(doodad); }
        void DoodadIsClean(Doodad* doodad) { this->_dirtyDoodads.erase(doodad); }
    private:
        Doodad* _CreateDoodad(Uint32 doodadId, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity& entity, std::string const& bodyName);
        void _ApiSpawn(Tools::Lua::CallHelper& helper);
        void _ApiKill(Tools::Lua::CallHelper& helper);
        void _ApiSet(Tools::Lua::CallHelper& helper);
        //void _ApiGet(Tools::Lua::CallHelper& helper); TODO ne pas oublier de prendre en compte les commands
        void _ApiCall(Tools::Lua::CallHelper& helper);
        void _ApiSetUdp(Tools::Lua::CallHelper& helper);
        void _ApiCallUdp(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
