#ifndef __SERVER_GAME_ENGINE_DOODADMANAGER_HPP__
#define __SERVER_GAME_ENGINE_DOODADMANAGER_HPP__

#include <luasel/Luasel.hpp>

#include "tools/lua/AWeakResourceRef.hpp"
#include "tools/lua/WeakResourceRefManager.hpp"

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
    public:
        struct WeakDoodadRef : public Tools::Lua::AWeakResourceRef<DoodadManager>
        {
            WeakDoodadRef() : doodadId(0), disabled(true) {}
            WeakDoodadRef(Uint32 doodadId) : doodadId(doodadId), disabled(false) {}
            virtual bool IsValid(DoodadManager const&) const { return this->doodadId && !this->disabled; }
            virtual void Invalidate(DoodadManager const&) { this->doodadId = 0; this->disabled = true; }
            virtual Luasel::Ref GetReference(DoodadManager& doodadManager) const;
            virtual std::string Serialize(DoodadManager const& doodadManager) const;
            bool operator <(WeakDoodadRef const& rhs) const;
            Uint32 doodadId;
            bool disabled;
        };

    private:
        Engine& _engine;
        std::map<Uint32 /* doodadId */, Doodad*> _doodads; // pointeur nul quand un doodad est désactivé
        std::map<Uint32 /* entityId */, std::list<Doodad*>> _doodadsByEntity;
        std::map<Uint32 /* entityId */, std::list<Doodad*>> _disabledDoodads;
        std::unordered_set<Doodad*> _dirtyDoodads;
        Uint32 _nextDoodadId;
        Tools::Lua::WeakResourceRefManager<WeakDoodadRef, DoodadManager>* _weakDoodadRefManager;

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
        Luasel::Ref GetLuaWrapperForDoodad(Uint32 doodadId);
        Tools::Lua::WeakResourceRefManager<WeakDoodadRef, DoodadManager>& GetWeakDoodadRefManager() { return *this->_weakDoodadRefManager; }
    private:
        Doodad& _GetDoodad(Uint32 doodadId) throw(std::runtime_error);
        Uint32 _RefToDoodadId(Luasel::Ref const& ref) throw(std::runtime_error);
        Doodad* _CreateDoodad(Uint32 doodadId, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity& entity, std::string const& bodyName);
        void _ApiSpawn(Luasel::CallHelper& helper);
        void _ApiKill(Luasel::CallHelper& helper);
        void _ApiSet(Luasel::CallHelper& helper);
        //void _ApiGet(Luasel::CallHelper& helper); TODO ne pas oublier de prendre en compte les commands
        void _ApiCall(Luasel::CallHelper& helper);
        void _ApiSetUdp(Luasel::CallHelper& helper);
        void _ApiCallUdp(Luasel::CallHelper& helper);
        void _ApiGetDoodadById(Luasel::CallHelper& helper);
        void _ApiGetWeakPointer(Luasel::CallHelper& helper);
    };

}}}

#endif
