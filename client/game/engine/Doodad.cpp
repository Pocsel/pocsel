#include "client/game/engine/Doodad.hpp"
#include "tools/lua/Interpreter.hpp"
#include "bullet/bullet-all.hpp"

namespace Client { namespace Game { namespace Engine {

    Doodad::Doodad(Tools::Lua::Interpreter& interpreter,
            Uint32 id,
            BodyType const* bodyType,
            Common::Physics::BodyCluster& bodyCluster,
            DoodadType const& type) :
        _type(type),
        _self(interpreter.MakeTable()),
        _body(0)
    {
        this->_self.Set("id", id);
        Tools::debug << "Doodad::Doodad: New doodad \"" << this->_type.GetName() << "\" spawned (id: " << id << ", pos: " << this->_physics.position.x << " " << this->_physics.position.y << " " << this->_physics.position.z << ")" << std::endl;

        if (bodyType)
            this->_body.reset(new Body(bodyCluster, bodyType));

        //_btBody->setActivationState(DISABLE_DEACTIVATION);
    }

    Doodad::~Doodad()
    {
        Tools::debug << "Doodad::~Doodad: Destroying doodad \"" << this->_type.GetName() << "\" (pos: " << this->_physics.position.x << " " << this->_physics.position.y << " " << this->_physics.position.z << ")" << std::endl;
    }

}}}
