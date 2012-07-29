#include "server/game/engine/PositionalEntity.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/DoodadManager.hpp"
#include "server/game/map/Map.hpp"

#include "server/network/PacketCreator.hpp"
#include "server/network/UdpPacket.hpp"

#include "common/physics/BodyCluster.hpp"

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

    void PositionalEntity::UpdatePhysics()
    {
        btRigidBody const& btBody = this->_bodyCluster->GetBody();
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
        if (this->_isDirty && this->_engine.GetDoodadManager().EntityHasDoodad(this->_id))
        {
            this->_isDirty = false;

            auto packet = Network::PacketCreator::EntityUpdate(this->_id, this->_physics);
            std::vector<Uint32> toDel;
            for (auto it = this->_players.begin(), ite = this->_players.end(); it != ite; ++it)
            {
                if (this->_engine.GetMap().HasPlayer(*it))
                {
                    auto packetCopy = std::unique_ptr<Common::Packet>(new Common::Packet(*packet));
                    this->_engine.SendPacket(*it, packetCopy);
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
