#include "client/game/engine/BodyType.hpp"

namespace Client { namespace Game { namespace Engine {

    BodyType::BodyType(std::vector<ShapeNode> const& shapes) :
        _shapes(shapes)
    {
        for (unsigned int i = 0; i < this->_shapes.size(); ++i)
        {
            this->_shapesMap[this->_shapes[i].name] = i;
            if (this->_shapes[i].parent == -1)
                this->_roots.push_back(i);
        }

#ifdef DEBUG
        this->_DumpTree(_roots, "");
#endif
    }

#ifdef DEBUG
    void BodyType::_DumpTree(std::vector<unsigned int> const& shapeNodes, std::string off)
    {
        auto it = shapeNodes.begin(), ite = shapeNodes.end();
        for (; it != ite; ++it)
        {
            std::cout << off << "> " << _shapes[*it].name << "\n";
            _DumpTree(_shapes[*it].children, off + "    ");
        }
    }
#endif

}}}
