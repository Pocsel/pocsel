#ifndef __CLIENT_GAME_ENGINE_ENTITY_HPP__
#define __CLIENT_GAME_ENGINE_ENTITY_HPP__

#include "common/physics/Node.hpp"

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
        float _updateFlag;
        mutable bool _updatedPhysics;
        mutable Common::Physics::Node _physics;
    public:
        Entity(Common::Physics::World& world, Uint32 id, Common::Physics::Node const& pos);
        ~Entity();

        void AddDoodad(Uint32 doodad);
        void RemoveDoodad(Uint32 doodad);

        void UpdatePhysics(std::vector<Common::Physics::Node> const& position);

        std::vector<Uint32> const& GetDoodads() const { return this->_doodads; }
        Common::Physics::BodyCluster& GetBodyCluster() { return *this->_bodyCluster; }

        void DeprecatePhysics() { this->_updatedPhysics = false; }
        Common::Physics::Node const& GetPhysics() const;

        void SetUpdateFlag(float value) { this->_updateFlag = value; }
        float GetUpdateFlag() const { return this->_updateFlag; }

        bool HasMoved() const { return true; } // TODO
    };

}}}

#endif
