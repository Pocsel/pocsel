#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/DoodadManager.hpp"
#include "server/game/map/Map.hpp"

#include "server/network/PacketCreator.hpp"
#include "server/network/UdpPacket.hpp"

#include "common/physics/BodyCluster.hpp"
#include "common/physics/World.hpp"

#include "bullet/bullet-all.hpp"

namespace Server { namespace Game { namespace Engine {

    PositionalEntity::PositionalEntity(
            Common::Physics::World& world,
            Engine& engine,
            Uint32 id,
            EntityType const& type,
            Common::Physics::Node const& pos) :
        Entity(engine, id, type),
        _bodyCluster(0)
    {
        this->_bodyCluster = new Common::Physics::BodyCluster(world, pos);
        this->_bodyCluster->SetUserData(this);
    }

    PositionalEntity::~PositionalEntity()
    {
        Tools::Delete(this->_bodyCluster);
    }

    void PositionalEntity::SetPosition(Common::Position const& pos)
    {
        Common::Physics::Node phys;
        phys.position = pos;
        phys.velocity = Common::Position(0, 0, 0);
        //phys.acceleration = Common::Position(0, 0, 0);
        phys.orientation = glm::quat();
        phys.angularVelocity = glm::vec3(0, 0, 0);

        std::vector<Common::Physics::Node> physics;
        physics.emplace_back(phys);
        this->_bodyCluster->SetPhysics(physics);
        this->UpdatePhysics();
        this->SetIsDirty(true);

        //this->_engine.GetDoodadManager().UpdatePhysicsFromDoodadOfEntity(this->_id);
    }

    void PositionalEntity::SetAccel(glm::dvec3 const& accel, double maxSpeed)
    {
        this->_bodyCluster->SetAccel(btVector3(accel.x, accel.y, accel.z), maxSpeed);
        this->UpdatePhysics();
        this->SetIsDirty(true);
    }

    void PositionalEntity::SetLocalAccel(glm::dvec3 const& accel, double maxSpeed)
    {
        this->_bodyCluster->SetLocalAccel(btVector3(accel.x, accel.y, accel.z), maxSpeed);
        this->UpdatePhysics();
        this->SetIsDirty(true);
    }

    void PositionalEntity::UpdatePhysics()
    {
        this->_clusterPhysics = this->_bodyCluster->GetClusterPhysics();
        this->_physics = this->_clusterPhysics[0];

        /*
        btRigidBody const& btBody = this->_bodyCluster->GetBtBody();
        Common::Physics::Node& physics = this->_physics;

        btTransform wt;
        btBody.getMotionState()->getWorldTransform(wt);

        btVector3 wpos = wt.getOrigin();
        physics.position = Common::Position(wpos.x(), wpos.y(), wpos.z());

        btQuaternion const& wrot = wt.getRotation();
        physics.orientation = glm::quat((float)wrot.w(), (float)wrot.x(), (float)wrot.y(), (float)wrot.z());

        btVector3 const& btVel = btBody.getLinearVelocity();
        physics.velocity = glm::vec3(btVel.x(), btVel.y(), btVel.z());

        btVector3 const& av = btBody.getAngularVelocity();
        physics.angularVelocity = glm::vec3(av.x(), av.y(), av.z());

        btVector3 const& accel = this->_bodyCluster->GetAccel();
        physics.acceleration = glm::dvec3(accel.x(), accel.y(), accel.z());
        if (accel != btVector3(0, 0, 0))
        {
            physics.accelerationIsLocal = this->_bodyCluster->IsAccelLocal();
            physics.maxSpeed = this->_bodyCluster->GetMaxSpeed();
        }
        */

        //std::cout << "positional entity: " <<
        //    wt.getOrigin().x() << ", " <<
        //    wt.getOrigin().y() << ", " <<
        //    wt.getOrigin().z() << "\n";

        //this->_bodyCluster->Dump();
    }

    void PositionalEntity::AddPlayer(Uint32 playerId)
    {
        this->_newPlayers.insert(playerId);
    }

    void PositionalEntity::RemovePlayer(Uint32 playerId)
    {
        this->_newPlayers.erase(playerId);
        this->_players.erase(playerId);
    }

    void PositionalEntity::UpdatePlayers()
    {
        if (!this->_engine.GetDoodadManager().EntityHasDoodad(this->_id))
            return;

        if (this->_isDirty)
        {
            this->_isDirty = false;

            auto packet = Network::PacketCreator::EntityUpdate(this->_id, this->_clusterPhysics);
            std::vector<Uint32> toDel;
            for (auto it = this->_players.begin(), ite = this->_players.end(); it != ite; ++it)
            {
                if (this->_engine.GetMap().HasPlayer(*it))
                {
                    auto packetCopy = std::unique_ptr<Network::UdpPacket>(new Network::UdpPacket(*packet));
                    this->_engine.SendUdpPacket(*it, packetCopy);
                }
                else
                {
                    toDel.push_back(*it);
                }
            }
            for (auto it = toDel.begin(), ite = toDel.end(); it != ite; ++it)
                this->_players.erase(*it);
        }

        for (auto it = this->_newPlayers.begin(), ite = this->_newPlayers.end(); it != ite; ++it)
            this->_players.insert(*it);
        this->_newPlayers.clear();
    }

}}}
