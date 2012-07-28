#ifndef __SERVER_GAME_ENGINE_ENTITYMANAGER_HPP__
#define __SERVER_GAME_ENGINE_ENTITYMANAGER_HPP__

#include "tools/lua/Ref.hpp"
#include "tools/lua/IWeakResourceRef.hpp"
#include "tools/lua/WeakResourceRefManager.hpp"
#include "server/game/engine/CallbackManager.hpp"
#include "common/Position.hpp"

namespace Tools { namespace Database {
    class IConnection;
}}
namespace Server { namespace Rcon {
    class EntityManager;
}}

namespace Server { namespace Game { namespace Engine {

    class Engine;
    class Entity;
    class PositionalEntity;
    class EntityType;

    class EntityManager :
        private boost::noncopyable
    {
    private:
        struct SpawnEvent
        {
            SpawnEvent(Uint32 pluginId, std::string const& entityName, Tools::Lua::Ref const& arg, Uint32 notificationCallbackId, bool hasPosition = false, Common::Position const& pos = Common::Position()) :
                pluginId(pluginId), entityName(entityName), arg(arg), notificationCallbackId(notificationCallbackId), hasPosition(hasPosition), pos(pos)
            {
            }
            Uint32 pluginId;
            std::string entityName;
            Tools::Lua::Ref arg;
            Uint32 notificationCallbackId;
            bool hasPosition;
            Common::Position pos;
        };
        struct KillEvent
        {
            KillEvent(Uint32 entityId, Tools::Lua::Ref const& arg, Uint32 notificationCallbackId) :
                entityId(entityId), arg(arg), notificationCallbackId(notificationCallbackId)
            {
            }
            Uint32 entityId;
            Tools::Lua::Ref arg;
            Uint32 notificationCallbackId;
        };
    public:
        struct WeakEntityRef : public Tools::Lua::IWeakResourceRef<EntityManager>
        {
            WeakEntityRef() : entityId(0), disabled(true) {}
            WeakEntityRef(Uint32 entityId) : entityId(entityId), disabled(false) {}
            virtual bool IsValid(EntityManager const&) const { return this->entityId && !this->disabled; }
            virtual void Invalidate(EntityManager const&) { this->entityId = 0; this->disabled = true; }
            virtual Tools::Lua::Ref GetReference(EntityManager const& entityManager) const;
            Uint32 entityId;
            bool disabled;
        };

    private:
        Engine& _engine;
        std::map<Uint32 /* pluginId */, std::map<std::string /* entityName */, EntityType*>> _entityTypes;
        std::map<Uint32 /* entityId */, Entity*> _entities; // pointeur nul quand une entité est désactivée
        std::map<Uint32 /* entityId */, PositionalEntity*> _positionalEntities; // pointeur nul quand une entité est désactivée
        std::map<Uint32 /* entityId */, PositionalEntity*> _disabledEntities;
        Uint32 _nextEntityId;
        std::list<SpawnEvent*> _spawnEvents;
        std::list<KillEvent*> _killEvents;
        Tools::Lua::WeakResourceRefManager<WeakEntityRef, EntityManager>* _weakEntityRefManager;

    public:
        EntityManager(Engine& engine);
        ~EntityManager();

        // peut retourner toutes les valeurs de CallbackManager::Result sauf CallbackNotFound (evidemment)
        // seul entry point Lua avec Engine::RconExecute()
        // si Error est retourné, l'entité a été supprimée
        CallbackManager::Result CallEntityFunction(Uint32 entityId, std::string const& function, Tools::Lua::Ref const& arg, Tools::Lua::Ref const& bonusArg, Tools::Lua::Ref* ret = 0);

        void AddSpawnEvent(Uint32 pluginId, std::string const& entityName, Tools::Lua::Ref const& arg, Uint32 notificationCallbackId, bool hasPosition = false, Common::Position const& pos = Common::Position());
        void AddKillEvent(Uint32 entityId, Tools::Lua::Ref const& arg, Uint32 notificationCallbackId);
        void DispatchSpawnEvents();
        void DispatchKillEvents();
        void Save(Tools::Database::IConnection& conn);
        void Load(Tools::Database::IConnection& conn);
        void BootstrapPlugin(Uint32 pluginId, Tools::Database::IConnection& conn);

        /*
         * Entity getters
         * Ne pas garder la reference/le pointeur, l'entité peut etre delete
         */
        Entity const& GetEntity(Uint32 entityId) const throw(std::runtime_error);
        Entity const& GetEntity(Tools::Lua::Ref const& ref) const throw(std::runtime_error);
        PositionalEntity& GetPositionalEntity(Uint32 entityId) throw(std::runtime_error);
        PositionalEntity const& GetPositionalEntity(Uint32 entityId) const throw(std::runtime_error);
        PositionalEntity const& GetPositionalEntity(Tools::Lua::Ref const& ref) const throw(std::runtime_error);
        PositionalEntity& GetDisabledEntity(Uint32 entityId) throw(std::runtime_error);
        PositionalEntity const& GetDisabledEntity(Uint32 entityId) const throw(std::runtime_error);

        bool IsEntityPositional(Uint32 entityId) const;
        void DisableEntity(Uint32 entityId, bool chunkUnloaded = true) throw(std::runtime_error);
        void EnableEntity(Uint32 entityId) throw(std::runtime_error);
        bool IsEntityTypePositional(Uint32 pluginId, std::string const& entityName) const;
        std::map<Uint32, PositionalEntity*> const& GetDisabledEntities() const { return this->_disabledEntities; }
        std::map<Uint32, PositionalEntity*> const& GetPositionalEntities() const { return this->_positionalEntities; }
        void SendDirtyPositionalEntities();
        Tools::Lua::WeakResourceRefManager<WeakEntityRef, EntityManager>& GetWeakEntityRefManager() { return *this->_weakEntityRefManager; }

        // rcon requests
        std::string RconGetEntities() const;
        void RconAddEntityTypes(Rcon::EntityManager& manager) const;

    private:
        Entity* _CreateEntity(Uint32 entityId, Uint32 pluginId, std::string entityName, bool hasPosition = false, Common::Position const& pos = Common::Position()) throw(std::runtime_error);
        void _DeleteEntity(Uint32 id, Entity* entity);
        void _ApiGetEntityById(Tools::Lua::CallHelper& helper);
        void _ApiSpawn(Tools::Lua::CallHelper& helper);
        void _ApiSave(Tools::Lua::CallHelper& helper);
        void _ApiLoad(Tools::Lua::CallHelper& helper);
        void _ApiKill(Tools::Lua::CallHelper& helper);
        void _ApiRegister(Tools::Lua::CallHelper& helper);

        // positionnal
        void _ApiRegisterPositional(Tools::Lua::CallHelper& helper);
        void _ApiSetPos(Tools::Lua::CallHelper& helper);
        void _ApiGetPos(Tools::Lua::CallHelper& helper);
        void _ApiSetSpeed(Tools::Lua::CallHelper& helper);
        void _ApiGetSpeed(Tools::Lua::CallHelper& helper);
        void _ApiSetAccel(Tools::Lua::CallHelper& helper);
        void _ApiGetAccel(Tools::Lua::CallHelper& helper);
    };

}}}

#endif
