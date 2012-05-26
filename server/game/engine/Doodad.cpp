#include "server/game/engine/Doodad.hpp"

namespace Server { namespace Game { namespace Engine {

    Doodad::Doodad(Uint32 id, Uint32 pluginId, std::string const& name, Uint32 entityId, PositionalEntity const& entity) :
        _id(id), _pluginId(pluginId), _name(name), _entityId(entityId), _entity(entity)
    {
        Tools::debug << "Doodad::Doodad: Doodad created (id " << this->_id << ", name \"" << this->_name << "\", pluginId " << this->_pluginId << ", entityId " << this->_entityId << ")." << std::endl;
    }

    Doodad::~Doodad()
    {
        Tools::debug << "Doodad::~Doodad: Doodad destroyed (id " << this->_id << ", name \"" << this->_name << "\", pluginId " << this->_pluginId << ", entityId " << this->_entityId << ")." << std::endl;
    }

    void Doodad::Disable()
    {
    }

    void Doodad::Enable()
    {
    }

}}}
