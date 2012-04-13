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
        virtual void Set(Vector2<float> const&) {}
        virtual void Set(Vector3<float> const&) {}
        virtual void Set(Matrix4<float> const&) {}
        virtual void Set(std::vector<glm::mat4x4> const&) {}
        virtual void Set(ITexture2D&) {}
    };

}}}

#endif
