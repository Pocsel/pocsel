#include "common/physics/BodyType.hpp"

#include "bullet/bullet-all.hpp"

namespace Common { namespace Physics {

    BodyType::BodyType(std::vector<ShapeNode> const& shapes) :
        _shapes(shapes)
    {
        for (unsigned int i = 0; i < this->_shapes.size(); ++i)
        {
            this->_shapesMap[this->_shapes[i].name] = i;

            if (this->_shapes[i].parent == -1)
                this->_roots.push_back(i);
        }
    }

    BodyType::~BodyType()
    {
        for (auto& bodyShape: this->_shapes)
        {
            Tools::Delete(bodyShape.shape);
            Tools::Delete(bodyShape.shapeDesc);
        }
    }

    void BodyType::CreateBtShapes()
    {
        for (auto& bodyShape: this->_shapes)
        {
            bodyShape.shape = bodyShape.shapeDesc->CreateShape().release();
        }
    }

    Int32 BodyType::GetNodeIndex(std::string const& nodeName) const
    {
        auto it = this->_shapesMap.find(nodeName);
        if (it == this->_shapesMap.end())
            return -1;
        return it->second;
    }

}}
