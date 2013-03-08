#pragma once

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    class ShaderProgram;

    class ShaderParameter : public IShaderParameter
    {
    private:
        GLint _uniform;

    public:
        ShaderParameter(Program& program, std::string const& identifier);
        virtual ~ShaderParameter();

        virtual bool IsUseable() const { return this->_uniform != 0; }

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
