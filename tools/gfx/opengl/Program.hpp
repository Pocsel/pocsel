#pragma once

#include "tools/gfx/opengl/opengl.hpp"
#include "tools/gfx/GLRenderer.hpp"
#include "tools/gfx/opengl/FragmentProgram.hpp"
#include "tools/gfx/opengl/ShaderParameter.hpp"
#include "tools/gfx/opengl/VertexProgram.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    class Program : public IProgram
    {
    private:
        GLRenderer& _renderer;

        std::map<std::string, std::unique_ptr<ShaderParameter>> _parameters;
        std::map<ShaderParameterUsage::Type, IShaderParameter*> _defaultParameters;
        GLint _attributes[VertexAttributeUsage::Max];
        IShaderParameter* _shaderParameters[VertexAttributeUsage::Max];

        GLuint _program;
        VertexProgram _vertexProgram;
        FragmentProgram _fragmentProgram;

    public:
        Program(GLRenderer& renderer, VertexProgram&& vertex, FragmentProgram&& fragment);
        virtual ~Program();

        virtual void SetAttributeUsage(std::string const& identifier, VertexAttributeUsage::Type usage);
        virtual void SetParameterUsage(ShaderParameterUsage::Type usage, std::string const& identifier);
        virtual IShaderParameter& GetParameter(std::string const& identifier);
        virtual void UpdateParameter(ShaderParameterUsage::Type usage);
        virtual void Update();
        virtual void Begin();
        virtual void End();

        GLuint GetProgram() const { return this->_program; }
        void UpdateParameters();
        GLint GetAttributeIndex(VertexAttributeUsage::Type usage) const { return this->_attributes[usage]; }
    };

}}}
