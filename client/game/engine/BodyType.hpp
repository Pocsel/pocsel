#ifndef __CLIENT_GAME_ENGINE_BODYTYPE_HPP__
#define __CLIENT_GAME_ENGINE_BODYTYPE_HPP__

#include "tools/lua/Ref.hpp"

namespace Client { namespace Game { namespace Engine {

    class BodyType :
        private boost::noncopyable
    {
    public:
        struct ShapeNode
        {
            std::string name;
            std::vector<Uint32> children;
            Int32 parent;
        };
    private:
        std::vector<ShapeNode> _shapes;
        std::vector<unsigned int> _roots;
        std::map<std::string /*name*/, ShapeNode*> _shapesMap;

    public:
        BodyType(std::vector<ShapeNode> const& shapes);

    };

}}}

#endif

