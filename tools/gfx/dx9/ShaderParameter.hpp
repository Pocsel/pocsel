#pragma once

#include "tools/gfx/DX9Renderer.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    class ShaderParameter : public IShaderParameter
    {
    private:
        IDirect3DDevice9* _device;
        ID3DXConstantTable* _vertexConstants;
        D3DXHANDLE _vertexParam;

        ID3DXConstantTable* _fragmentConstants;
        D3DXHANDLE _fragmentParam;
        D3DXCONSTANT_DESC _desc;

    public:
        ShaderParameter(Program& program, std::string const& identifier);
        virtual ~ShaderParameter();

        virtual bool IsUseable() const { return this->_vertexParam != nullptr || this->_fragmentParam != nullptr; }

        virtual void Set(int value);
        virtual void Set(float value);
        virtual void Set(glm::detail::tvec2<float> const& vector);
        virtual void Set(glm::detail::tvec3<float> const& vector);
        virtual void Set(glm::detail::tvec4<float> const& vector);
        virtual void Set(glm::detail::tmat4x4<float> const& matrix, bool isProjection = false);
        virtual void Set(std::vector<glm::mat4x4> const& matrices);
        virtual void Set(ITexture2D& texture);
        virtual void Set(ITexture2D& texture, ISamplerState& sampler);
    };

}}}
