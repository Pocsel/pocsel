#ifndef __SERVER_GAME_ENGINE_BODYTYPE_HPP__
#define __SERVER_GAME_ENGINE_BODYTYPE_HPP__

#include "tools/lua/Ref.hpp"
#include "common/physics/BodyType.hpp"

namespace Server { namespace Game { namespace Engine {

    class BodyType :
        public Common::Physics::BodyType
    {
    private:
        std::string _name;
        Uint32 _pluginId;
        Uint32 _id;

    public:
        BodyType(std::string const& name, Uint32 pluginId, Uint32 id, Tools::Lua::Ref const& prototype);
        std::string const& GetName() const { return this->_name; }
        Uint32 GetPluginId() const { return this->_pluginId; }
        Uint32 GetId() const { return this->_id; }

    private:
        void _FillShapeTree(Tools::Lua::Ref const& shapeTree, std::vector<unsigned int>& result, int parent);
        unsigned int _BuildShapeNode(Tools::Lua::Ref const& shapeTree, int parent);
    };

}}}

#endif
