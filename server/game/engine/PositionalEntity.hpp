#ifndef __SERVER_GAME_ENGINE_POSITIONALENTITY_HPP__
#define __SERVER_GAME_ENGINE_POSITIONALENTITY_HPP__

#include "common/physics/Node.hpp"
#include "server/game/engine/Entity.hpp"

namespace Common { namespace Physics {
    class World;
    class BodyCluster;
}}

namespace Server { namespace Game { namespace Engine {

    class PositionalEntity :
        public Entity
    {
    private:
        Common::Physics::BodyCluster* _bodyCluster;
        Common::Physics::Node _physics;
        std::vector<Common::Physics::Node> _clusterPhysics;
        std::set<Uint32> _players;
        std::set<Uint32> _newPlayers;
        bool _isDirty;

    public:
        PositionalEntity(
                Common::Physics::World& world,
                Engine& engine,
                Uint32 id,
                EntityType const& type,
                Common::Physics::Node const& pos);
        ~PositionalEntity();

        Common::Position const& GetPosition() const { return this->_physics.position; }
        void SetPosition(Common::Position const& pos);// { this->_physics.position = pos; }

        void SetAccel(glm::dvec3 const& accel, double maxSpeed);
        void SetLocalAccel(glm::dvec3 const& accel, double maxSpeed);

        //glm::dvec3 const& GetSpeed() const { return this->_physics.velocity; }
        //void SetSpeed(glm::dvec3 const& speed);// { this->_physics.velocity = speed; }
        //glm::dvec3 const& GetAccel() const { return this->_physics.acceleration; }
        //void SetAccel(glm::dvec3 const& accel) { this->_physics.acceleration = accel; }

        //Common::Physics::Node const& GetPhysics() const;// { return this->_physics; }
        //void SetPhysics(Common::Physics::Node const& p);// { this->_physics = p; }

        Common::Physics::Node const& GetPhysics() const { return this->_physics; }
        Common::Physics::BodyCluster& GetBodyCluster() { return *this->_bodyCluster; }

        void UpdatePhysics();
        void SetIsDirty(bool isDirty) { this->_isDirty = isDirty; }
        bool GetIsDirty() const { return this->_isDirty; }
        void UpdatePlayers();

        void AddPlayer(Uint32 playerId);
        void RemovePlayer(Uint32 playerId);
        std::set<Uint32> const& GetPlayers() { return this->_players; }
        std::set<Uint32> const& GetNewPlayers() { return this->_newPlayers; }
    };

}}}

#endif
