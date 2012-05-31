#include "server/game/engine/BodyType.hpp"

#include "tools/lua/Iterator.hpp"

namespace Server { namespace Game { namespace Engine {

    BodyType::BodyType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& prototype) :
        _name(name), _pluginId(pluginId)//, _prototype(prototype)
    {
        if (prototype["shapeTree"].Exists())
        {
            if (!prototype["shapeTree"].IsTable())
                throw std::runtime_error("Client.Model.Register: Field \"shapeTree\" must be of type table");
            Tools::Lua::Ref shapeTree = prototype.GetTable("shapeTree");
            this->_FillShapeTree(shapeTree, this->_shapeTree);
        }

        _DumpTree(this->_shapeTree, "");
    }

    void BodyType::_FillShapeTree(Tools::Lua::Ref const& shapeTree, std::vector<ShapeTreeNode>& result)
    {
        Tools::Lua::Iterator it = shapeTree.Begin(), ite = shapeTree.End();
        for (; it != ite; ++it)
        {
            Tools::Lua::Ref node = it.GetValue();
            if (!node.IsTable())
                throw std::runtime_error("Client.BodyType.Register: each shape node must be of type table");
            result.emplace_back();
            this->_BuildShapeNode(node, result.back());
        }
    }

    void BodyType::_BuildShapeNode(Tools::Lua::Ref& shapeTree, ShapeTreeNode& node)
    {
        Tools::Lua::Iterator it = shapeTree.Begin(), ite = shapeTree.End();
        for (; it != ite; ++it)
        {
            std::string key =  it.GetKey().ToString();

            if (key == "name")
            {
                Tools::Lua::Ref name = it.GetValue();
                if (!name.IsString())
                    throw std::runtime_error("Client.BodyType.Register: Field \"name\" must be of type string");
                node.name = name.ToString();
                if (this->_shapeTreeMap.count(node.name))
                    throw std::runtime_error("Client.BodyType.Register: each node name must be different (\"" + node.name + "\" appears more than one time)");

            }
            else if (key == "children")
            {
                Tools::Lua::Ref children = it.GetValue();
                if (!children.IsTable())
                    throw std::runtime_error("Client.Model.Register: Field \"children\" must be of type table");
                this->_FillShapeTree(it.GetValue(), node.children);
            }
        }
        if (node.name == "")
            throw std::runtime_error("A shapeTree node needs a name");
        this->_shapeTreeMap[node.name] = &node;
    }

    void BodyType::_DumpTree(std::vector<ShapeTreeNode> const& shapeNodes, std::string off)
    {
        auto it = shapeNodes.begin(), ite = shapeNodes.end();
        for (; it != ite; ++it)
        {
            std::cout << off << "> " << it->name << "\n";
            _DumpTree(it->children, off + "    ");
        }
    }

}}}
