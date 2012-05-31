#ifndef __SERVER_GAME_ENGINE_BODYTYPE_HPP__
#define __SERVER_GAME_ENGINE_BODYTYPE_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class BodyType :
        private boost::noncopyable
    {
    public:
        struct ShapeTreeNode
        {
            std::string name;
            std::vector<ShapeTreeNode> children;
        };
    private:
        std::string _name;
        Uint32 _pluginId;
        //Tools::Lua::Ref _prototype;
        std::vector<ShapeTreeNode> _shapeTree;
        std::map<std::string /*name*/, ShapeTreeNode*> _shapeTreeMap;

    public:
        BodyType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& prototype);
        std::string const& GetName() const { return this->_name; }
        Uint32 GetPluginId() const { return this->_pluginId; }
//        Tools::Lua::Ref const& GetPrototype() const { return this->_prototype; }
//        void SetPrototype(Tools::Lua::Ref const& prototype) { this->_prototype = prototype; }

    private:
        void _FillShapeTree(Tools::Lua::Ref const& shapeTree, std::vector<ShapeTreeNode>& result);
        void _BuildShapeNode(Tools::Lua::Ref& shapeTree, ShapeTreeNode& node);
        void _DumpTree(std::vector<ShapeTreeNode> const& shapeNodes, std::string off);
    };

}}}

#endif
