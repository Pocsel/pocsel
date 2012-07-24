#ifndef __COMMON_PHYSICS_BODYTYPE_HPP__
#define __COMMON_PHYSICS_BODYTYPE_HPP__

#include "common/physics/Node.hpp"
#include "common/physics/IShapeDesc.hpp"

class btCollisionShape;

namespace Common { namespace Physics {

    class IShapeDesc;

    class BodyType
    {
    public:
        struct ShapeNode
        {
            ShapeNode() :
                shape(0), shapeDesc(0), mass(1), friction(0.5), restitution(0) {}
            ShapeNode(Int32 parent) :
                shape(0), shapeDesc(0), mass(1), friction(0.5), restitution(0), parent(parent) {}

            std::string name;
            std::vector<Uint32> children;
            Node position;
            btCollisionShape* shape;
            IShapeDesc* shapeDesc;
            float mass;
            float friction;
            float restitution;
            Int32 parent;
        };
        static const unsigned int ShapesMax = 50;
    protected:
        std::vector<ShapeNode> _shapes;
        std::vector<Uint32> _roots;
        std::map<std::string /*name*/, Uint32> _shapesMap;

    public:
        BodyType(std::vector<ShapeNode> const& shapes);
        ~BodyType();
        std::vector<ShapeNode> const& GetShapes() const { return this->_shapes; }
        std::vector<Uint32> const& GetRoots() const { return this->_roots; }
        std::map<std::string, unsigned int> const& GetShapesMap() const { return this->_shapesMap; }

        void CreateBtShapes();

    protected:
        BodyType() {}
        void _DumpTree(std::vector<unsigned int> const& shapeNodes, std::string off)
        {
            auto it = shapeNodes.begin(), ite = shapeNodes.end();
            for (; it != ite; ++it)
            {
                Tools::debug << off <<
                    "> " << _shapes[*it].name <<
                    " -> " << _shapes[*it].shapeDesc->ToString() << "\n";

                _DumpTree(_shapes[*it].children, off + "    ");
            }
        }
    };

}}

#endif
