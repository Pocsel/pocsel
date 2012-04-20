#include "tools/precompiled.hpp"

#include <IL/il.h>

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/IndexBuffer.hpp"
#include "tools/renderers/opengl/RenderTarget.hpp"
#include "tools/renderers/opengl/ShaderProgramCg.hpp"
#include "tools/renderers/opengl/ShaderProgramNull.hpp"
#include "tools/renderers/opengl/Texture2D.hpp"
#include "tools/renderers/opengl/VertexBuffer.hpp"
#include "tools/renderers/GLRenderer.hpp"
#include "tools/logger/Logger.hpp"

#ifdef _MSC_VER
# pragma warning(disable: 4244)
#endif

namespace Tools { namespace Renderers {

    namespace {
        CGcontext _cgGlobalContext = 0;
        int _cgGlobalNbReferences = 0;

        void ErrCallback()
        {
            auto error = cgGetError();
            if (error == CG_NO_ERROR || error == CG_CANNOT_DESTROY_PARAMETER_ERROR)
                return;
            if (error == CG_COMPILER_ERROR)
                Tools::error << "Cg compiler:\n" << cgGetLastListing(_cgGlobalContext) << "\n";
            throw std::runtime_error(
                "An internal Cg call failed (Code: "
                + ToString(error) + "): "
                + cgGetErrorString(cgGetError()));
        }

        void InitDevIL()
        {
            static bool initialized = false;
            if (!initialized)
            {
                ilInit();

                ilEnable(IL_ORIGIN_SET);
                ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

                ilEnable(IL_TYPE_SET);
                ilTypeFunc(IL_UNSIGNED_BYTE);

                ilEnable(IL_FORMAT_SET);
                ilFormatFunc(IL_RGBA);
                ilEnable(IL_ALPHA);

                initialized = true;
            }
        }
    }

    void GLRenderer::Initialise()
    {
        RenderState rs;
        rs.state = RenderState::None;
        rs.matrixMode = -1;
        rs.target = 0;
        this->_PushState(rs);

        GLenum error = glewInit();
        if (error != GLEW_OK)
            throw std::runtime_error(ToString("glewInit() failed: ") + ToString(glewGetErrorString(error)));
        if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
            this->_useShaders = false;

        Tools::log << "Glew version: " << (char const*)glewGetString(GLEW_VERSION) << "\n";
        Tools::log << "OpenGL version: " << (char const*)glGetString(GL_VERSION) << "\n";

        GLCHECK(glEnable(GL_BLEND));
        //GLCHECK(glEnable(GL_ALPHA_TEST));
        GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        //GLCHECK(glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        if (this->_useShaders)
        {
            if (_cgGlobalContext == 0)
                _cgGlobalContext = cgCreateContext();
            this->_cgContext = _cgGlobalContext;
            ++_cgGlobalNbReferences;
            cgSetErrorCallback(&ErrCallback);
            cgGLSetDebugMode(CG_FALSE);
            cgSetParameterSettingMode(this->_cgContext, CG_DEFERRED_PARAMETER_SETTING);
            cgGLRegisterStates(this->_cgContext);
            cgGLSetManageTextureParameters(this->_cgContext, CG_TRUE);
        }
        InitDevIL();
    }

    void GLRenderer::Shutdown()
    {
        if (this->_useShaders && --_cgGlobalNbReferences == 0)
        {
            cgDestroyContext(this->_cgContext);
            _cgGlobalContext = 0;
        }
    }

    std::unique_ptr<IVertexBuffer> GLRenderer::CreateVertexBuffer()
    {
        return std::unique_ptr<IVertexBuffer>(new OpenGL::VertexBuffer());
    }

    std::unique_ptr<IIndexBuffer> GLRenderer::CreateIndexBuffer()
    {
        return std::unique_ptr<IIndexBuffer>(new OpenGL::IndexBuffer(*this));
    }

    std::unique_ptr<Renderers::IRenderTarget> GLRenderer::CreateRenderTarget(glm::uvec2 const& imgSize)
    {
        return std::unique_ptr<IRenderTarget>(new OpenGL::RenderTarget(*this, imgSize));
    }

    std::unique_ptr<ITexture2D> GLRenderer::CreateTexture2D(PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize, void const* mipmapData)
    {
        return std::unique_ptr<ITexture2D>(new OpenGL::Texture2D(*this, format, size, data, imgSize, mipmapData));
    }

    std::unique_ptr<ITexture2D> GLRenderer::CreateTexture2D(std::string const& imagePath)
    {
        return std::unique_ptr<ITexture2D>(new OpenGL::Texture2D(*this, imagePath));
    }

    std::unique_ptr<IShaderProgram> GLRenderer::CreateProgram(std::string const& effect)
    {
        if (this->_useShaders)
            return std::unique_ptr<IShaderProgram>(new OpenGL::ShaderProgramCg(*this, effect));
        else
            return std::unique_ptr<IShaderProgram>(new OpenGL::ShaderProgramNull(*this));
    }

    // Drawing
    void GLRenderer::Clear(int clearFlags)
    {
        GLCHECK(glClear(
            (clearFlags & ClearFlags::Color ? GL_COLOR_BUFFER_BIT : 0) |
            (clearFlags & ClearFlags::Depth ? GL_DEPTH_BUFFER_BIT : 0) |
            (clearFlags & ClearFlags::Stencil ? GL_STENCIL_BUFFER_BIT : 0)));
    }

    void GLRenderer::BeginDraw2D(IRenderTarget* target)
    {
        RenderState rs;
        rs.state = RenderState::Draw2D;
        rs.target = target;
        this->_PushState(rs);

        GLCHECK(glDisable(GL_DEPTH_TEST));
        GLCHECK(glDisable(GL_CULL_FACE));
    }

    void GLRenderer::EndDraw2D()
    {
        this->_PopState();
    }

    void GLRenderer::BeginDraw(IRenderTarget* target)
    {
        RenderState rs;
        rs.state = RenderState::Draw3D;
        rs.target = target;
        rs.model = this->_currentState->model;
        rs.view = this->_currentState->view;
        rs.projection = this->_currentState->projection;
        rs.modelViewProjection = this->_currentState->modelViewProjection;
        this->_PushState(rs);

        GLCHECK(glEnable(GL_DEPTH_TEST));
        GLCHECK(glEnable(GL_CULL_FACE));
    }

    void GLRenderer::EndDraw()
    {
        this->_PopState();
    }

    void GLRenderer::UpdateCurrentParameters()
    {
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelMatrix);
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewMatrix);
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
        if (this->_currentProgram != 0)
            this->_currentProgram->UpdateCurrentPass();
    }

    void GLRenderer::DrawElements(Uint32 count, DataType::Type indicesType, void const* indices, DrawingMode::Type mode)
    {
        this->UpdateCurrentParameters();
        if (indices == 0 && this->bindedIndexBuffer != 0)
            indicesType = this->bindedIndexBuffer->GetType();
        GLCHECK(::glDrawElements(OpenGL::GetDrawingMode(mode), count, OpenGL::GetTypeFromDataType(indicesType), indices));
    }

    void GLRenderer::DrawVertexBuffer(Uint32 offset, Uint32 count, DrawingMode::Type mode)
    {
        this->UpdateCurrentParameters();
        GLCHECK(::glDrawArrays(OpenGL::GetDrawingMode(mode), offset, count));
    }

    // Matrices
    void GLRenderer::SetModelMatrix(glm::detail::tmat4x4<float> const& matrix)
    {
        this->_currentState->model = matrix;
        if (this->_currentProgram != 0)
            this->_currentState->modelViewProjection = this->_currentState->projection * this->_currentState->view * this->_currentState->model;
    }

    void GLRenderer::SetViewMatrix(glm::detail::tmat4x4<float> const& matrix)
    {
        this->_currentState->view = matrix;
        if (this->_currentProgram != 0)
        {
            this->_currentState->modelViewProjection = this->_currentState->projection * this->_currentState->view * this->_currentState->model;
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ViewMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
        }
    }

    void GLRenderer::SetProjectionMatrix(glm::detail::tmat4x4<float> const& matrix)
    {
        this->_currentState->projection = matrix;
        if (this->_currentProgram != 0)
        {
            this->_currentState->modelViewProjection = this->_currentState->projection * this->_currentState->view * this->_currentState->model;
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ProjectionMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
        }
    }

    // States
    void GLRenderer::SetScreenSize(glm::uvec2 const& size)
    {
        this->_screenSize = size;
        GLCHECK(::glViewport(0, 0, size.x, size.y));
    }

    void GLRenderer::SetClearColor(Color4f const& color)
    {
        GLCHECK(::glClearColor(color.r, color.g, color.b, color.a));
    }

    void GLRenderer::SetClearDepth(float value)
    {
        GLCHECK(::glClearDepth(value));
    }

    void GLRenderer::SetClearStencil(int value)
    {
        GLCHECK(::glClearStencil(value));
    }

    void GLRenderer::SetNormaliseNormals(bool normalise)
    {
        if (normalise)
            GLCHECK(glEnable(GL_NORMALIZE));
        else
            GLCHECK(glDisable(GL_NORMALIZE));
    }

    void GLRenderer::SetDepthTest(bool enabled)
    {
        if (enabled)
            GLCHECK(glEnable(GL_DEPTH_TEST));
        else
            GLCHECK(glDisable(GL_DEPTH_TEST));
    }

    void GLRenderer::SetDepthWrite(bool enabled)
    {
        GLCHECK(glDepthMask(enabled ? GL_TRUE : GL_FALSE));
    }

    void GLRenderer::SetCullFace(bool enabled)
    {
        if (enabled)
            GLCHECK(glEnable(GL_CULL_FACE));
        else
            GLCHECK(glDisable(GL_CULL_FACE));
    }

    void GLRenderer::SetRasterizationMode(RasterizationMode::Type rasterizationMode)
    {
        switch (rasterizationMode)
        {
        case RasterizationMode::Point:
            GLCHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_POINT));
            break;

        case RasterizationMode::Line:
            GLCHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
            break;

        case RasterizationMode::Fill:
            GLCHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
            break;
        }
    }

    void GLRenderer::SetMatrixMode(unsigned int mode)
    {
        if (this->_currentState->matrixMode == mode)
            return;
        this->_currentState->matrixMode = mode;
        GLCHECK(glMatrixMode(mode));
    }

    void GLRenderer::_PushState(GLRenderer::RenderState const& state)
    {
        RenderState rsOld;
        if (this->_states.size() > 0)
            rsOld = this->_states.front();

        this->_states.push_front(state);
        auto& rs = this->_states.front();
        this->_currentState = &rs;

        if (rs.target != 0)
        {
            rs.target->Bind();
            GLCHECK(::glViewport(0, 0, rs.target->GetSize().x, rs.target->GetSize().y));
        }

        if (rs.state == GLRenderer::RenderState::Draw2D)
        {
            rs.view = glm::translate<float>(0, 0, 1);
            auto size = rs.target == 0 ? this->_screenSize : rs.target->GetSize();
            rs.projection = glm::ortho<float>(0, size.x, size.y, 0);
            rs.modelViewProjection = rs.projection * rs.view * rs.model;
        }
        else
        {
            rs.view = rsOld.view;
            rs.projection = rsOld.projection;
            rs.modelViewProjection = rs.projection * rs.view * rsOld.model;
        }
    }

    void GLRenderer::_PopState()
    {
        auto rsOld = this->_states.front();
        this->_states.pop_front();
        auto& rs = this->_states.front();
        this->_currentState = &rs;

        if (rsOld.target != 0)
        {
            if (rs.target != 0)
            {
                rs.target->Bind();
                GLCHECK(::glViewport(0, 0, rs.target->GetSize().x, rs.target->GetSize().y));
            }
            else
            {
                bool reset = true;
                for (auto it = this->_states.begin(), ite = this->_states.end(); it != ite; ++it)
                {
                    if (it->target)
                    {
                        it->target->Bind();
                        GLCHECK(::glViewport(0, 0, it->target->GetSize().x, it->target->GetSize().y));
                        reset = false;
                        break;
                    }
                }
                if (reset)
                {
                    GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0));
                    GLCHECK(::glViewport(0, 0, this->_screenSize.x, this->_screenSize.y));
                }
            }
        }

        if (rsOld.state != rs.state)
        {
            if (rs.state == RenderState::Draw3D)
            {
                GLCHECK(glEnable(GL_DEPTH_TEST));
                GLCHECK(glEnable(GL_CULL_FACE));
            }
            else if (rs.state == RenderState::Draw2D)
            {
                GLCHECK(glDisable(GL_DEPTH_TEST));
                GLCHECK(glDisable(GL_CULL_FACE));
            }
        }
    }

}}
