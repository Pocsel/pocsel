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
        std::map<Uint32 /* doodadId */, Doodad*> _doodads;
        std::map<Uint32 /* entityId */, std::list<Doodad*>> _doodadsByEntities;
        std::map<Uint32 /* entityId */, std::list<Doodad*>> _disabledDoodads;
        Uint32 _nextDoodadId;

    public:
        DoodadManager(Engine& engine);
        ~DoodadManager();
        void Save(Tools::Database::IConnection& conn);
        void Load(Tools::Database::IConnection& conn);
        void DeleteDoodadsOfEntity(Uint32 entityId);
        void DisableDoodadsOfEntity(Uint32 entityId);
        void EnableDoodadsOfEntity(Uint32 entityId);
    private:
        void _CreateDoodad(Uint32 doodadId, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity const& entity);
        void _ApiSpawn(Tools::Lua::CallHelper& helper);
        void _ApiKill(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
