#ifndef __CLIENT_GAME_ENGINE_DOODAD_HPP__
#define __CLIENT_GAME_ENGINE_DOODAD_HPP__

#include "client/game/engine/DoodadType.hpp"
#include "tools/lua/Ref.hpp"

#include "common/physics/Node.hpp"

namespace Common { namespace Physics {
    class BodyCluster;
}}

class btRigidBody;

namespace Client { namespace Game { namespace Engine {

    class BodyType;
    class Body;

    class Doodad :
        private boost::noncopyable
    {
    private:
        DoodadType const& _type;
        Tools::Lua::Ref _self;
        Common::Physics::BodyCluster const& bodyCluster;
        std::unique_ptr<Body> _body;

        float _updateFlag;

    public:
        Doodad(Tools::Lua::Interpreter& interpreter,
                Uint32 id,
                BodyType const* bodyType,
                Common::Physics::BodyCluster& bodyCluster,
                DoodadType const& type);
        ~Doodad();
        DoodadType const& GetType() const { return this->_type; }
        Tools::Lua::Ref const& GetSelf() const { return this->_self; }

        //Common::Position const& GetPosition() const { return this->_physics.position; }
        //void SetPosition(Common::Position const& pos) { this->_physics.position = pos; }
        //glm::dvec3 const& GetSpeed() const { return this->_physics.velocity; }
        //void SetSpeed(glm::dvec3 const& speed) { this->_physics.velocity = speed; }
        //glm::dvec3 const& GetAccel() const { return this->_physics.acceleration; }
        //void SetAccel(glm::dvec3 const& accel) { this->_physics.acceleration = accel; }

        //Common::Physics::Node const& GetPhysics() const { return this->_physics; }
        //void SetPhysics(Common::Physics::Node const& p) { this->_physics = p; }

        //Common::Physics::Node& GetPhysics() { return this->_physics; }
        //btRigidBody& GetBtBody() { return *this->_btBody; }

        void SetUpdateFlag(float value) { this->_updateFlag = value; }
        float GetUpdateFlag() const { return this->_updateFlag; }
    };

}}}

#endif
