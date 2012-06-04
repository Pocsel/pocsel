#ifndef __CLIENT_GAME_ENGINE_BODYTYPE_HPP__
#define __CLIENT_GAME_ENGINE_BODYTYPE_HPP__

#include "tools/lua/Ref.hpp"

namespace Client { namespace Game { namespace Engine {

    class BodyType
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
        std::map<std::string /*name*/, unsigned int> _shapesMap;

    public:
        BodyType() {}
        BodyType(std::vector<ShapeNode> const& shapes);

    private:
#ifdef DEBUG
        void _DumpTree(std::vector<unsigned int> const& shapeNodes, std::string off);
#endif
    };

}}}

#endif

