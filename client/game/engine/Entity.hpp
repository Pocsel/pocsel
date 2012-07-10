#ifndef __CLIENT_GAME_ENGINE_ENTITY_HPP__
#define __CLIENT_GAME_ENGINE_ENTITY_HPP__

namespace Common { namespace Physics {
    class World;
    class BodyCluster;
    struct Node;
}}

namespace Client { namespace Game { namespace Engine {

    class Entity
    {
    private:
        Uint32 _id;
        Common::Physics::BodyCluster* _bodyCluster;
        std::vector<Uint32> _doodads;
    public:
        Entity(Common::Physics::World& world, Uint32 id, Common::Physics::Node const& pos);
        ~Entity();

        void AddDoodad(Uint32 doodad);
        void RemoveDoodad(Uint32 doodad);

        void SetPosition(Common::Physics::Node const& position);

        std::vector<Uint32> const& GetDoodads() const { return this->_doodads; }
        Common::Physics::BodyCluster& GetBodyCluster() { return *this->_bodyCluster; }
    };

}}}

#endif
