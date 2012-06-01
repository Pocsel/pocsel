#include "server/game/engine/Body.hpp"
#include "server/game/engine/BodyType.hpp"


namespace Server { namespace Game { namespace Engine {

    Body::Body(BodyType const& bodyType) :
        _type(bodyType)
    {
        for (auto it = this->_type.GetShapesMap().begin(), ite = this->_type.GetShapesMap().end(); it != ite; ++it)
        {
            this->_nodes.insert(std::pair<std::string, BodyNode>(it->first, BodyNode()));
        }
    }

}}}
