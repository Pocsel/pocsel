#include "server/precompiled.hpp"

#include "server/game/engine/BodyType.hpp"

#include "tools/lua/Iterator.hpp"
#include "tools/lua/Function.hpp"
#include "common/FieldUtils.hpp"

#include "bullet/bullet-all.hpp"

namespace Server { namespace Game { namespace Engine {

    BodyType::BodyType(std::string const& name, Uint32 pluginId, Uint32 id, Tools::Lua::Ref const& prototype) :
        Common::Physics::BodyType(),
        _name(name),
        _pluginId(pluginId),//, _prototype(prototype)
        _id(id)
    {
        _shapes.reserve(ShapesMax);
        if (prototype["shapeTree"].Exists())
        {
            if (!prototype["shapeTree"].IsTable())
                throw std::runtime_error("Client.Body.Register: Field \"shapeTree\" must be of type table");
            Tools::Lua::Ref shapeTree = prototype.GetTable("shapeTree");
            this->_FillShapeTree(shapeTree, this->_roots, -1);
        }

#ifdef DEBUG
        this->_DumpTree(_roots, "");
#endif
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
        auto idx = (unsigned int)this->_shapes.size();
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
            else if (key == "position")
            {
                Tools::Lua::Ref position = it.GetValue();
                if (!position.IsTable())
                    throw std::runtime_error("Client.Body.Register: Field \"position\" must be of type table");
                node.position.position.x = position[1].ToNumber();
                node.position.position.y = position[2].ToNumber();
                node.position.position.z = position[3].ToNumber();
            }
            else if (key == "orientation")
            {
                Tools::Lua::Ref orientation = it.GetValue();
                if (!orientation.IsTable())
                    throw std::runtime_error("Client.Body.Register: Field \"orientation\" must be of type table");
                glm::vec3 pitchyawroll;
                pitchyawroll.x = orientation[2].ToNumber();
                pitchyawroll.y = orientation[1].ToNumber();
                pitchyawroll.z = orientation[3].ToNumber();
                node.position.orientation = glm::quat(pitchyawroll); // pitch, yaw, roll
            }
            else if (key == "shape")
            {
                Tools::Lua::Ref shape = it.GetValue();
                if (!shape.IsTable())
                    throw std::runtime_error("Client.Body.Register: Field \"shape\" must be of type table");
                node.shape = _BuildShape(shape);
            }
        }
        if (!Common::FieldUtils::IsRegistrableType(node.name))
            throw std::runtime_error("Client.Body.Register: A shapeTree node needs a name");
        this->_shapesMap[node.name] = idx;
        return idx;
    }

    btCollisionShape* BodyType::_BuildShape(Tools::Lua::Ref& lua)
    {
        Tools::Lua::Ref shapeTypeLua = lua["shapeType"];
        if (!shapeTypeLua.IsString())
            throw std::runtime_error("Client.Body.Register: Field \"shapeType\" must be of type string");
        std::string shapeType = shapeTypeLua.ToString();

        if (shapeType == "box")
            return _BuildBoxShape(lua);
        else if (shapeType == "sphere")
            return _BuildSphereShape(lua);
        else if (shapeType == "empty")
            return new btEmptyShape();

        throw std::runtime_error("Client.Body.Register: you need a correct shapeType");
    }

    btCollisionShape* BodyType::_BuildBoxShape(Tools::Lua::Ref& lua)
    {
        Tools::Lua::Ref halfExtentsLua = lua["halfExtents"];
        if (!halfExtentsLua.IsTable())
            throw std::runtime_error("Client.Body.Register: Field \"halfExtents\" must be of type table");
        btVector3 halfExtents(
                halfExtentsLua[1].ToNumber(),
                halfExtentsLua[2].ToNumber(),
                halfExtentsLua[3].ToNumber()
                );
        return new btBoxShape(halfExtents);
    }

    btCollisionShape* BodyType::_BuildSphereShape(Tools::Lua::Ref& lua)
    {
        Tools::Lua::Ref radiusLua = lua["radius"];
        if (!radiusLua.IsNumber())
            throw std::runtime_error("Client.Body.Register: Field \"radius\" must be of type number");
        return new btSphereShape(radiusLua.ToNumber());
    }

}}}
