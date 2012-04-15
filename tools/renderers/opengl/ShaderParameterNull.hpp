#ifndef __TOOLS_RENDERERS_OPENGL_SHADERPARAMETERNULL_HPP__
#define __TOOLS_RENDERERS_OPENGL_SHADERPARAMETERNULL_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    class ShaderParameterNull : public IShaderParameter
    {
    public:
        virtual void Set(bool) {}
        virtual void Set(int) {}
        virtual void Set(float) {}
        virtual void Set(Color4<float> const&) {}
        virtual void Set(glm::detail::tvec2<float> const&) {}
        virtual void Set(glm::detail::tvec3<float> const&) {}
        virtual void Set(glm::detail::tmat4x4<float> const&) {}
        virtual void Set(std::vector<glm::mat4x4> const&) {}
        virtual void Set(ITexture2D&) {}
    };

}}}

#endif
