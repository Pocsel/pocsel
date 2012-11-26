#include "client/game/engine/Body.hpp"
#include "client/game/engine/BodyType.hpp"

namespace Client { namespace Game { namespace Engine {

    Body::Body(Common::Physics::BodyCluster& parent, BodyType const& bodyType) :
        Common::Physics::Body(parent, bodyType)
    {
        this->_boundNodes.resize(this->_nodes.size());
    }

    bool Body::BindNode(std::string const& nodeName, std::shared_ptr<glm::mat4x4> const& boundNode)
    {
        unsigned int idx = 0;
        for (auto& node: this->_type.GetShapes())
        {
            if (node.name == nodeName)
            {
                this->_boundNodes[idx] = boundNode;
                return true;
            }
        }
        return false;
    }

}}}
