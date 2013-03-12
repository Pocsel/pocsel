#pragma once

#include "tools/gfx/dx9/directx.hpp"
#include "tools/gfx/DX9Renderer.hpp"
#include "tools/gfx/dx9/FragmentProgram.hpp"
#include "tools/gfx/dx9/VertexProgram.hpp"

namespace Tools { namespace Gfx {

    namespace DX9 {

        class Program : public IProgram
        {
        private:
            DX9Renderer& _renderer;
            VertexProgram _vertexProgram;
            FragmentProgram _fragmentProgram;

            std::map<ShaderParameterUsage::Type, IShaderParameter*> _defaultParameters;
            std::map<std::string, std::unique_ptr<IShaderParameter>> _parameters;

        public:
            Program(DX9Renderer& renderer, std::string const& vertexShader, std::string const& pixelShader);
            Program(DX9Renderer& renderer, VertexProgram&& vertexProgram, FragmentProgram&& fragmentProgram);
            virtual ~Program();

            virtual void SetAttributeUsage(std::string const& identifier, VertexAttributeUsage::Type usage);
            virtual void SetParameterUsage(ShaderParameterUsage::Type usage, std::string const& identifier);
            virtual IShaderParameter& GetParameter(std::string const& identifier);
            virtual void UpdateParameter(ShaderParameterUsage::Type usage);
            virtual void Update();
            virtual void Begin();
            virtual void End();

            DX9Renderer& GetRenderer() { return this->_renderer; }
            VertexProgram& GetVertexProgram() { return this->_vertexProgram; }
            FragmentProgram& GetFragmentProgram() { return this->_fragmentProgram; }
        };

}}}
