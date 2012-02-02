#ifndef __TOOLS_RENDERERS_OPENGL_SHADERPROGRAM_HPP__
#define __TOOLS_RENDERERS_OPENGL_SHADERPROGRAM_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers {

    class GLRenderer;

    namespace OpenGL {

        class ShaderProgram : public IShaderProgram
        {
        private:
            GLuint _id;
            GLRenderer& _renderer;
            int _nbTextures;

            GLint _mvpId;
            GLint _modelId;
            GLint _viewId;
            GLint _projectionId;

        public:
            ShaderProgram(GLRenderer& renderer, std::string const& vertexShader, std::string const& fragmentShader);
            virtual ~ShaderProgram();

            GLuint GetID() const { return this->_id; }
            virtual std::unique_ptr<IShaderParameter> GetParameter(std::string const& identifier);
            virtual void SetParameterUsage(std::string const& identifier, ShaderParameterUsage::Type usage);
            void UpdateParameter(ShaderParameterUsage::Type usage);
            virtual void Activate();
        };

}}}

#endif
