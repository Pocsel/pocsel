#pragma once

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/GLRenderer.hpp"
#include "tools/renderers/opengl/FragmentProgram.hpp"
#include "tools/renderers/opengl/VertexProgram.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    class Program : public IProgram
    {
    private:
        GLRenderer& _renderer;

        int _nbTextures;

        std::map<std::string, std::unique_ptr<IShaderParameter>> _parameters;
        GLint _attributes[VertexAttributeUsage::Max];
        IShaderParameter* _shaderParameters[VertexAttributeUsage::Max];

        GLuint _program;
        VertexProgram _vertexProgram;
        FragmentProgram _fragmentProgram;

    public:
        Program(GLRenderer& renderer, VertexProgram&& vertex, FragmentProgram&& fragment);
        virtual ~Program();

        virtual void SetAttributeUsage(std::string const& identifier, VertexAttributeUsage::Type usage);
        virtual IShaderParameter& GetParameter(std::string const& identifier);
        virtual void UpdateParameter(ShaderParameterUsage::Type usage) {}
        virtual void Begin();
        virtual void End();

        GLuint GetProgram() const { return this->_program; }
        void UpdateParameters();
        GLint GetAttributeIndex(VertexAttributeUsage::Type usage) const { return this->_attributes[usage]; }
    };

}}}
