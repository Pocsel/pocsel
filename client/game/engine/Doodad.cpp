#include <luasel/Luasel.hpp>

#include "client/game/engine/Doodad.hpp"
#include "client/game/engine/Body.hpp"
#include "client/game/engine/Entity.hpp"

#include "bullet/bullet-all.hpp"

namespace Client { namespace Game { namespace Engine {

    Doodad::Doodad(Luasel::Interpreter& interpreter,
            Uint32 id,
            BodyType const* bodyType,
            Entity& entity,
            DoodadType const& type) :
        _type(type),
        _self(interpreter.MakeTable()),
        _entity(entity)
    {
        this->_self.Set("id", id);
        Tools::debug << "Doodad::Doodad: New doodad \"" << this->_type.GetName() << "\" spawned (id: " << id << ")\n";//", pos: " << this->_physics.position.x << " " << this->_physics.position.y << " " << this->_physics.position.z << ")" << std::endl;

        if (bodyType)
            this->_body.reset(new Body(entity.GetBodyCluster(), *bodyType));

        //_btBody->setActivationState(DISABLE_DEACTIVATION);
    }

    Doodad::~Doodad()
    {
        Tools::debug << "Doodad::~Doodad: Destroying doodad \"" << this->_type.GetName() << ")\n"; // \" (pos: " << this->_physics.position.x << " " << this->_physics.position.y << " " << this->_physics.position.z << ")" << std::endl;
    }

    //void Doodad::UpdatePosition(Common::Physics::Node const& node)
    //{
    //    this->_body->UpdatePosition();
    //}

    Common::Physics::Node const& Doodad::GetPhysics() const
    {
        return this->_entity.GetPhysics();
    }

    float Doodad::GetUpdateFlag() const
    {
        return this->_entity.GetUpdateFlag();
    }

    bool Doodad::HasMoved() const
    {
        return this->_entity.HasMoved();
    }

}}}
