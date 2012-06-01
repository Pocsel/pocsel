#ifndef __SERVER_GAME_ENGINE_BODYTYPE_HPP__
#define __SERVER_GAME_ENGINE_BODYTYPE_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class BodyType :
        private boost::noncopyable
    {
    public:
        struct ShapeNode
        {
            ShapeNode(int parent) : parent(parent) {}
            std::string name;
            std::vector<Uint32> children;
            Int32 parent;
        };
        static const unsigned int ShapesMax = 50;
    private:
        std::string _name;
        Uint32 _pluginId;
        std::vector<ShapeNode> _shapes;
        std::vector<unsigned int> _roots;
        std::map<std::string /*name*/, ShapeNode*> _shapesMap;

    public:
        BodyType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& prototype);
        std::string const& GetName() const { return this->_name; }
        Uint32 GetPluginId() const { return this->_pluginId; }
        std::map<std::string, ShapeNode*> const& GetShapesMap() const { return this->_shapesMap; }

    private:
        void _FillShapeTree(Tools::Lua::Ref const& shapeTree, std::vector<unsigned int>& result, int parent);
        unsigned int _BuildShapeNode(Tools::Lua::Ref& shapeTree, int parent);
        void _DumpTree(std::vector<unsigned int> const& shapeNodes, std::string off);
    };

}}}

#endif
