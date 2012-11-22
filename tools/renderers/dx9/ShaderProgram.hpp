#ifndef __TOOLS_RENDERERS_DX9_SHADERPROGRAM_HPP__
#define __TOOLS_RENDERERS_DX9_SHADERPROGRAM_HPP__

#include "tools/renderers/dx9/directx.hpp"
#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers {

    class DX9Renderer;

    namespace DX9 {

        class ShaderProgram : public IShaderProgram
        {
        private:
            DX9Renderer& _renderer;
            ID3DXEffect* _effect;
            ComPtr<IDirect3DVertexShader9> _vertexShader;
            ComPtr<IDirect3DPixelShader9> _pixelShader;
            D3DXHANDLE _technique;
            int _nbTextures;

            D3DXHANDLE _mvp;
            D3DXHANDLE _vp;
            D3DXHANDLE _mv;
            D3DXHANDLE _model;
            D3DXHANDLE _view;
            D3DXHANDLE _projection;
            D3DXHANDLE _modelInverse;
            D3DXHANDLE _viewInverse;
            D3DXHANDLE _projectionInverse;
            D3DXHANDLE _mvpInverse;
            D3DXHANDLE _viewProjectionInverse;
            D3DXHANDLE _worldViewInverseTranspose;

            std::map<std::string, std::unique_ptr<IShaderParameter>> _parameters;

            UINT _pass;
            UINT _passCount;

        public:
            ShaderProgram(DX9Renderer& renderer, std::string const& effect);
            ShaderProgram(DX9Renderer& renderer, std::string const& vertexShader, std::string const& vsFunc, std::string const& pixelShader, std::string const& psFunc);
            virtual ~ShaderProgram();

            virtual IShaderParameter& GetParameter(std::string const& identifier);
            virtual IShaderParameter& GetParameterFromSemantic(std::string const& semantic);
            virtual void SetParameterUsage(std::string const& identifier, ShaderParameterUsage::Type usage);
            virtual void UpdateParameter(ShaderParameterUsage::Type usage);
            virtual void UpdateCurrentPass();
            virtual void BeginPass();
            virtual bool EndPass();

            ID3DXEffect* GetEffect() const { return this->_effect; }
        };

}}}

#endif
