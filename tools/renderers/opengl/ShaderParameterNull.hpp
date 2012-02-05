#ifndef __TOOLS_RENDERERS_OPENGL_SHADERPARAMETERNULL_HPP__
#define __TOOLS_RENDERERS_OPENGL_SHADERPARAMETERNULL_HPP__

#include <stdexcept>
#include <string>

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    class ShaderParameterNull : public IShaderParameter
    {
    public:
        virtual std::string const& GetIdentifier() const { throw std::runtime_error("Don't call GetIdentifier()."); }
        virtual void Set(bool) {}
        virtual void Set(int) {}
        virtual void Set(float) {}
        virtual void Set(Color4<float> const&) {}
        virtual void Set(Vector2<float> const&) {}
        virtual void Set(Vector3<float> const&) {}
        virtual void Set(Matrix4<float> const&) {}
        virtual void Set(ITexture2D&) {}
    };

}}}

#endif
