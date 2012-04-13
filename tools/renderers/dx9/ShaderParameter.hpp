#ifndef __TOOLS_RENDERERS_DX9_SHADERPARAMETER_HPP__
#define __TOOLS_RENDERERS_DX9_SHADERPARAMETER_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    class ShaderProgram;

    class ShaderParameter : public IShaderParameter
    {
    private:
        ID3DXEffect* _effect;
        D3DXHANDLE _param;

    public:
        ShaderParameter(ShaderProgram& program, std::string const& identifier);
        ShaderParameter(ShaderProgram& program, D3DXHANDLE parameter);
        virtual ~ShaderParameter();

        virtual void Set(bool value);
        virtual void Set(int value);
        virtual void Set(float value);
        virtual void Set(Color4<float> const& color);
        virtual void Set(glm::detail::tvec2<float> const& vector);
        virtual void Set(glm::detail::tvec3<float> const& vector);
        virtual void Set(glm::detail::tmat4x4<float> const& matrix);
        virtual void Set(ITexture2D& texture);
    };

}}}

#endif
