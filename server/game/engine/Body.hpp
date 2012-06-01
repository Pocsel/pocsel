#ifndef __SERVER_GAME_ENGINE_BODY_HPP__
#define __SERVER_GAME_ENGINE_BODY_HPP__

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Engine {

    class Engine;
    class BodyType;

    class Body :
        private boost::noncopyable
    {
    public:
        struct BodyNode
        {
            BodyNode() : _dirty(false) {}
            glm::vec3 position;
            glm::vec3 orientation;
            glm::vec3 size;
            bool _dirty;
        };
    private:
        BodyType const& _type;
        std::map<std::string, BodyNode> _nodes;

    public:
        Body(BodyType const& bodyType);
    };

}}}

#endif
