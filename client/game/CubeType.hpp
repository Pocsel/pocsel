#ifndef __CLIENT_GAME_CUBETYPE_HPP__
#define __CLIENT_GAME_CUBETYPE_HPP__

#include "common/CubeType.hpp"
#include "tools/renderers/utils/material/LuaMaterial.hpp"

namespace Tools { namespace Lua {
    class Interpreter;
    class Ref;
}}

namespace Client {
    class Client;
    namespace Resources {
        class ResourceManager;
    }
}

namespace Client { namespace Game {

    class CubeType : public Common::CubeType
    {
    public:
        std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial> top;
        std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial> bottom;
        std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial> right;
        std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial> left;
        std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial> front;
        std::unique_ptr<Tools::Renderers::Utils::Material::LuaMaterial> back;

        explicit CubeType(Common::CubeType const& cubeType);

        CubeType(CubeType&& rhs);
        CubeType& operator =(CubeType&& rhs);

        void LoadMaterial(Resources::ResourceManager& resourceMgr, Tools::Lua::Ref const& prototype);

    private:
        CubeType(CubeType const&) CPP11_DELETE;
        CubeType& operator =(CubeType const&) CPP11_DELETE;
    };

}}

#endif
