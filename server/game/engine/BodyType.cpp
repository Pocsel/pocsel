#include "server/game/engine/BodyType.hpp"

#include "tools/lua/Iterator.hpp"
#include "common/FieldValidator.hpp"

namespace Server { namespace Game { namespace Engine {

    BodyType::BodyType(std::string const& name, Uint32 pluginId, Tools::Lua::Ref const& prototype) :
        _name(name), _pluginId(pluginId)//, _prototype(prototype)
    {
        _shapes.reserve(ShapesMax);
        if (prototype["shapeTree"].Exists())
        {
            if (!prototype["shapeTree"].IsTable())
                throw std::runtime_error("Client.Body.Register: Field \"shapeTree\" must be of type table");
            Tools::Lua::Ref shapeTree = prototype.GetTable("shapeTree");
            this->_FillShapeTree(shapeTree, this->_roots, -1);
        }

        this->_DumpTree(_roots, "");
    }

    void BodyType::_FillShapeTree(Tools::Lua::Ref const& shapeTree, std::vector<unsigned int>& result, int parent)
    {
        Tools::Lua::Iterator it = shapeTree.Begin(), ite = shapeTree.End();
        for (; it != ite; ++it)
        {
            Tools::Lua::Ref node = it.GetValue();
            if (!node.IsTable())
                throw std::runtime_error("Client.Body.Register: each shape node must be of type table");
            result.push_back(this->_BuildShapeNode(node, parent));
        }
    }

    unsigned int BodyType::_BuildShapeNode(Tools::Lua::Ref& shapeTree, int parent)
    {
        unsigned int idx = this->_shapes.size();
        if (idx == ShapesMax)
            throw std::runtime_error("Client.Body.Register: Too many shapes (" + Tools::ToString(ShapesMax)+ " max)");
        this->_shapes.push_back(ShapeNode(parent));
        ShapeNode& node = this->_shapes.back();
        Tools::Lua::Iterator it = shapeTree.Begin(), ite = shapeTree.End();
        for (; it != ite; ++it)
        {
            std::string key =  it.GetKey().ToString();

            if (key == "name")
            {
                Tools::Lua::Ref name = it.GetValue();
                if (!name.IsString())
                    throw std::runtime_error("Client.Body.Register: Field \"name\" must be of type string");
                node.name = name.ToString();
                if (this->_shapesMap.count(node.name))
                    throw std::runtime_error("Client.Body.Register: each node name must be different (\"" + node.name + "\" appears more than one time)");

            }
            else if (key == "children")
            {
                Tools::Lua::Ref children = it.GetValue();
                if (!children.IsTable())
                    throw std::runtime_error("Client.Body.Register: Field \"children\" must be of type table");
                this->_FillShapeTree(it.GetValue(), node.children, idx);
            }
        }
        if (!Common::FieldValidator::IsRegistrableType(node.name))
            throw std::runtime_error("Client.Body.Register: A shapeTree node needs a name");
        this->_shapesMap[node.name] = &node;
        return idx;
    }

    void BodyType::_DumpTree(std::vector<unsigned int> const& shapeNodes, std::string off)
    {
        auto it = shapeNodes.begin(), ite = shapeNodes.end();
        for (; it != ite; ++it)
        {
            std::cout << off << "> " << _shapes[*it].name << "\n";
            _DumpTree(_shapes[*it].children, off + "    ");
        }
    }

}}}
