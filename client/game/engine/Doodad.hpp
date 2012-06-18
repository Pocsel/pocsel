#ifndef __CLIENT_GAME_ENGINE_DOODAD_HPP__
#define __CLIENT_GAME_ENGINE_DOODAD_HPP__

#include "client/game/engine/DoodadType.hpp"
#include "tools/lua/Ref.hpp"

#include "common/physics/Node.hpp"

class btRigidBody;

namespace Client { namespace Game { namespace Engine {

    class Doodad :
        private boost::noncopyable
    {
    private:
        DoodadType const& _type;
        Tools::Lua::Ref _self;
        Common::Physics::Node _physics;
        btRigidBody* _btBody;

        float _updateFlag;

    public:
        Doodad(Tools::Lua::Interpreter& interpreter, Uint32 id, Common::Physics::Node const& physics, DoodadType const& type);
        ~Doodad();
        DoodadType const& GetType() const { return this->_type; }
        Tools::Lua::Ref const& GetSelf() const { return this->_self; }

        Common::Position const& GetPosition() const { return this->_physics.position.r; }
        void SetPosition(Common::Position const& pos) { this->_physics.position.r = pos; }
        glm::dvec3 const& GetSpeed() const { return this->_physics.position.v; }
        void SetSpeed(glm::dvec3 const& speed) { this->_physics.position.v = speed; }
        glm::dvec3 const& GetAccel() const { return this->_physics.position.a; }
        void SetAccel(glm::dvec3 const& accel) { this->_physics.position.a = accel; }

        Common::Physics::Node const& GetPhysics() const { return this->_physics; }
        void SetPhysics(Common::Physics::Node const& p) { this->_physics = p; }

        Common::Physics::Node& GetPhysics() { return this->_physics; }
        btRigidBody& GetBtBody() { return *this->_btBody; }

        void SetUpdateFlag(float value) { this->_updateFlag = value; }
        float GetUpdateFlag() const { return this->_updateFlag; }
    };

}}}

#endif
