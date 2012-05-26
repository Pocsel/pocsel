#ifndef __SERVER_GAME_ENGINE_DOODAD_HPP__
#define __SERVER_GAME_ENGINE_DOODAD_HPP__

namespace Server { namespace Game { namespace Engine {

    class PositionalEntity;

    class Doodad :
        private boost::noncopyable
    {
    private:
        Uint32 _id;
        Uint32 _pluginId;
        std::string _name;
        Uint32 _entityId;
        PositionalEntity const& _entity;

    public:
        Doodad(Uint32 id, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity const& entity);
        ~Doodad();
        Uint32 GetId() const { return this->_id; }
        Uint32 GetEntityId() const { return this->_entityId; }
        void Disable();
        void Enable();
    };

}}}

#endif
