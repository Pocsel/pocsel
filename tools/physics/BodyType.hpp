#ifndef __TOOLS_PHYSICS_BODYTYPE_HPP__
#define __TOOLS_PHYSICS_BODYTYPE_HPP__

namespace Tools { namespace Physics {

    class BodyType
    {
    public:
        struct ShapeNode
        {
            ShapeNode() {}
            ShapeNode(Int32 parent) : parent(parent) {}
            std::string name;
            std::vector<Uint32> children;
            Int32 parent;
        };
        static const unsigned int ShapesMax = 50;
    protected:
        std::vector<ShapeNode> _shapes;
        std::vector<unsigned int> _roots;
        std::map<std::string /*name*/, unsigned int> _shapesMap;

    public:
        BodyType(std::vector<ShapeNode> const& shapes);
        std::vector<ShapeNode> const& GetShapes() const { return this->_shapes; }
        std::vector<unsigned int> const& GetRoots() const { return this->_roots; }
        std::map<std::string, unsigned int> const& GetShapesMap() const { return this->_shapesMap; }

    protected:
        BodyType() {}
        void _DumpTree(std::vector<unsigned int> const& shapeNodes, std::string off)
        {
            auto it = shapeNodes.begin(), ite = shapeNodes.end();
            for (; it != ite; ++it)
            {
                Tools::debug << off << "> " << _shapes[*it].name << "\n";
                _DumpTree(_shapes[*it].children, off + "    ");
            }
        }
    };

}}

#endif
