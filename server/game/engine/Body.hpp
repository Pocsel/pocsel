#ifndef __SERVER_GAME_ENGINE_BODY_HPP__
#define __SERVER_GAME_ENGINE_BODY_HPP__

#include "tools/lua/Ref.hpp"
#include "tools/physics/Node.hpp"

namespace Server { namespace Game { namespace Engine {

    class Engine;
    class BodyType;

    class Body :
        private boost::noncopyable
    {
    public:
        struct BodyNode
        {
            BodyNode() : dirty(false) {}
            Tools::Physics::Node node;
            bool dirty;
        };
    private:
        BodyType const& _type;
        std::vector<BodyNode> _nodes;

    public:
        Body(BodyType const& bodyType);
        BodyType const& GetType() const { return this->_type; }
        std::vector<BodyNode> const& GetNodes() const { return this->_nodes; }
    };

}}}

#endif
