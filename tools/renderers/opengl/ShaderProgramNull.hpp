#ifndef __TOOLS_RENDERERS_OPENGL_SHADERPROGRAMNULL_HPP__
#define __TOOLS_RENDERERS_OPENGL_SHADERPROGRAMNULL_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers {

    class GLRenderer;

    namespace OpenGL {

        class ShaderProgramNull : public IShaderProgram
        {
        private:
            GLRenderer& _renderer;

        public:
            ShaderProgramNull(GLRenderer& renderer);

            virtual std::unique_ptr<IShaderParameter> GetParameter(std::string const& identifier);
            virtual void SetParameterUsage(std::string const&, ShaderParameterUsage::Type) {}
            virtual void UpdateParameter(ShaderParameterUsage::Type usage);
            virtual void Activate();
        };

}}}

#endif
