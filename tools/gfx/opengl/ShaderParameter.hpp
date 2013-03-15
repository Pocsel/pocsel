#pragma once

#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    class Program;

    class ShaderParameter : public IShaderParameter
    {
    private:
        GLint _uniform;
        enum
        {
            None,
            Int,
            Float,
            Float2,
            Float3,
            Float4,
            Float4x4,
            Float4x4array,
            Texture
        } _type;
        std::unique_ptr<char[]> _value;

    public:
        ShaderParameter(Program& program, std::string const& identifier);
        virtual ~ShaderParameter();

        virtual bool IsUseable() const { return this->_uniform != 0; }

        virtual void Set(int value);
        virtual void Set(float value);
        virtual void Set(glm::detail::tvec2<float> const& vector);
        virtual void Set(glm::detail::tvec3<float> const& vector);
        virtual void Set(glm::detail::tvec4<float> const& vector);
        virtual void Set(glm::detail::tmat4x4<float> const& matrix, bool isProjection = false);
        virtual void Set(std::vector<glm::mat4x4> const& matrices);
        virtual void Set(ITexture2D& texture);

        void Update();
    };

}}}
