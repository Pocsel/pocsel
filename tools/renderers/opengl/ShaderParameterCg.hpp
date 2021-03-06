#ifndef __TOOLS_RENDERERS_OPENGL_SHADERPARAMETER_HPP__
#define __TOOLS_RENDERERS_OPENGL_SHADERPARAMETER_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    class ShaderProgramCg;

    class ShaderParameterCg : public IShaderParameter
    {
    private:
        CGparameter _param;
        std::unique_ptr<std::vector<float>> _preBuffer;

    public:
        ShaderParameterCg(ShaderProgramCg& program, std::string const& identifier);
        ShaderParameterCg(CGparameter parameter);
        virtual ~ShaderParameterCg();

        virtual bool IsUseable() const { return this->_param != 0; }

        virtual void Set(bool value);
        virtual void Set(int value);
        virtual void Set(float value);
        virtual void Set(glm::detail::tvec2<float> const& vector);
        virtual void Set(glm::detail::tvec3<float> const& vector);
        virtual void Set(glm::detail::tvec4<float> const& vector);
        virtual void Set(glm::detail::tmat4x4<float> const& matrix, bool isProjection = false);
        virtual void Set(std::vector<glm::mat4x4> const& matrices);
        virtual void Set(ITexture2D& texture);
    };

}}}

#endif
