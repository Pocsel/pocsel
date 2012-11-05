#include "client/precompiled.hpp"

#include <luasel/Luasel.hpp>

#include "client/game/CubeType.hpp"
#include "client/resources/ResourceManager.hpp"

using Tools::Renderers::Utils::Material::LuaMaterial;

namespace Client { namespace Game {

    CubeType::CubeType(Common::CubeType const& cubeType) :
        Common::CubeType(cubeType)
    {
    }

    CubeType::CubeType(CubeType&& rhs) :
        Common::CubeType(rhs)
    {
        *this = std::move(rhs);
    }

    CubeType& CubeType::operator =(CubeType&& rhs)
    {
        if (this != &rhs)
        {
            this->top.reset(rhs.top.release());
            this->bottom.reset(rhs.bottom.release());
            this->right.reset(rhs.right.release());
            this->left.reset(rhs.left.release());
            this->front.reset(rhs.front.release());
            this->back.reset(rhs.back.release());
        }
        return *this;
    }

    void CubeType::LoadMaterial(Resources::ResourceManager& resourceMgr, Luasel::Ref const& prototype)
    {
        auto effects = prototype["materials"];
        this->top.reset(new LuaMaterial(effects["top"]));
        this->bottom.reset(new LuaMaterial(effects["bottom"]));
        this->right.reset(new LuaMaterial(effects["right"]));
        this->left.reset(new LuaMaterial(effects["left"]));
        this->front.reset(new LuaMaterial(effects["front"]));
        this->back.reset(new LuaMaterial(effects["back"]));
    }

}}
