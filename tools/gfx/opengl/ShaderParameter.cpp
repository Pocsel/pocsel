#include "tools/precompiled.hpp"

#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/opengl/opengl.hpp"
#include "tools/gfx/opengl/Texture2D.hpp"
#include "tools/gfx/opengl/Program.hpp"
#include "tools/gfx/opengl/ShaderParameter.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    ShaderParameter::ShaderParameter(Program& program, std::string const& identifier) :
        _type(None)
    {
        if (glGetUniformLocation != nullptr)
            GLCHECK(this->_uniform = glGetUniformLocation(program.GetProgram(), identifier.c_str()));
        //else if (glGetUniformLocationARB != nullptr)
        //    GLCHECK(this->_uniform = glGetUniformLocationARB(program.GetProgram(), identifier.c_str()));
        else
            throw std::runtime_error("glGetUniformLocation unsupported");
    }

    ShaderParameter::~ShaderParameter()
    {
    }

    void ShaderParameter::Set(int value)
    {
        if (this->_value.get() == nullptr)
        {
            this->_type = Int;
            this->_value.reset(new char[sizeof(int)]);
        }
        std::memcpy(this->_value.get(), &value, sizeof(int));
        //GLCHECK(glUniform1i(this->_uniform, value));
    }

    void ShaderParameter::Set(float value)
    {
        if (this->_value.get() == nullptr)
        {
            this->_type = Float;
            this->_value.reset(new char[sizeof(float)]);
        }
        std::memcpy(this->_value.get(), &value, sizeof(float));
        //GLCHECK(glUniform1f(this->_uniform, value));
    }

    void ShaderParameter::Set(glm::detail::tvec2<float> const& vector)
    {
        if (this->_value.get() == nullptr)
        {
            this->_type = Float2;
            this->_value.reset(new char[2 * sizeof(float)]);
        }
        std::memcpy(this->_value.get(), &vector, 2 * sizeof(float));
        //GLCHECK(glUniform2fv(this->_uniform, 1, (float const*)&vector));
    }

    void ShaderParameter::Set(glm::detail::tvec3<float> const& vector)
    {
        if (this->_value.get() == nullptr)
        {
            this->_type = Float3;
            this->_value.reset(new char[3 * sizeof(float)]);
        }
        std::memcpy(this->_value.get(), &vector, 3 * sizeof(float));
        //GLCHECK(glUniform3fv(this->_uniform, 1, (float const*)&vector));
    }

    void ShaderParameter::Set(glm::detail::tvec4<float> const& vector)
    {
        if (this->_value.get() == nullptr)
        {
            this->_type = Float4;
            this->_value.reset(new char[4 * sizeof(float)]);
        }
        std::memcpy(this->_value.get(), &vector, 4 * sizeof(float));
        //GLCHECK(glUniform4fv(this->_uniform, 1, (float const*)&vector));
    }

    void ShaderParameter::Set(glm::detail::tmat4x4<float> const& matrix, bool)
    {
        auto tmp = glm::transpose(matrix);
        if (this->_value.get() == nullptr)
        {
            this->_type = Float4x4;
            this->_value.reset(new char[4 * 4 * sizeof(float)]);
        }
        std::memcpy(this->_value.get(), &tmp, 4 * 4 * sizeof(float));
        //GLCHECK(glUniform4fv(this->_uniform, 4, (float const*)&tmp));
    }

    void ShaderParameter::Set(std::vector<glm::mat4x4> const& matrices)
    {
        GLsizei size = (GLsizei)(4 * matrices.size());
        this->_type = Float4x4array;
        this->_value.reset(new char[sizeof(GLsizei) + size]);
        GLsizei* ptr = (GLsizei*)this->_value.get();
        *ptr = size;
        ptr++;
        std::memcpy((void*)ptr, matrices.data(), size);
    }

    void ShaderParameter::Set(ITexture2D& texture)
    {
        if (this->_value.get() == nullptr)
        {
            this->_type = Texture;
            this->_value.reset(new char[sizeof(Texture2D*)]);
        }
        (*(ITexture2D**)this->_value.get()) = &texture;
    }

    void ShaderParameter::Update()
    {
        //assert(this->_value.get() != nullptr && "aucune valeur !");
        switch (this->_type)
        {
        case None: break;
        case Int:       GLCHECK(glUniform1i(this->_uniform, *(int*)this->_value.get())); break;
        case Float:     GLCHECK(glUniform1fv(this->_uniform, 1, (float*)this->_value.get())); break;
        case Float2:    GLCHECK(glUniform2fv(this->_uniform, 1, (float*)this->_value.get())); break;
        case Float3:    GLCHECK(glUniform3fv(this->_uniform, 1, (float*)this->_value.get())); break;
        case Float4:    GLCHECK(glUniform4fv(this->_uniform, 1, (float*)this->_value.get())); break;
        case Float4x4:  GLCHECK(glUniform4fv(this->_uniform, 4, (float*)this->_value.get())); break;
        case Float4x4array:
            {
                GLsizei* ptr = (GLsizei*)this->_value.get();
                GLsizei size = *ptr;
                ptr++;
                GLCHECK(glUniform4fv(this->_uniform, size, (float const*)ptr));
            }
            break;
        case Texture:
            {
                Texture2D* tex = *(Texture2D**)this->_value.get();
                assert(tex->GetBindID() >= 0 && "Faut bind la texture avant.");
                GLCHECK(glUniform1i(this->_uniform, tex->GetBindID()));
            }
            break;
        }
    }

}}}
