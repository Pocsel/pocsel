#include "common/physics/BodyType.hpp"
#include "common/physics/ShapeDesc.hpp"

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
        for (auto it = this->_shapes.begin(), ite = this->_shapes.end(); it != ite; ++it)
        {
            Tools::Delete(it->shape);
            Tools::Delete(it->shapeDesc);
        }
    }

}}
