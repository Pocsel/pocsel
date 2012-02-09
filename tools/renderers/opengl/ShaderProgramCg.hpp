#ifndef __TOOLS_RENDERERS_OPENGL_SHADERPROGRAM_HPP__
#define __TOOLS_RENDERERS_OPENGL_SHADERPROGRAM_HPP__

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers {

    class GLRenderer;

    namespace OpenGL {

        class ShaderProgramCg : public IShaderProgram
        {
        private:
            GLRenderer& _renderer;
            CGcontext _ctx;
            CGeffect _effect;
            CGtechnique _technique;
            int _nbTextures;

            CGparameter _mvp;
            CGparameter _mv;
            CGparameter _model;
            CGparameter _view;
            CGparameter _projection;

            CGpass _pass;

        public:
            ShaderProgramCg(GLRenderer& renderer, std::string const& effect);
            virtual ~ShaderProgramCg();

            virtual std::unique_ptr<IShaderParameter> GetParameter(std::string const& identifier);
            virtual void SetParameterUsage(std::string const& identifier, ShaderParameterUsage::Type usage);
            virtual void UpdateParameter(ShaderParameterUsage::Type usage);
            virtual void BeginPass();
            virtual bool EndPass();

            CGcontext GetContext() const { return this->_ctx; }
            CGeffect GetEffect() const { return this->_effect; }
        };

}}}

#endif
