#include "tools/precompiled.hpp"

#include <IL/il.h>

#include "tools/gfx/opengl/opengl.hpp"
#include "tools/gfx/opengl/IndexBuffer.hpp"
#include "tools/gfx/opengl/RenderTarget.hpp"
#include "tools/gfx/opengl/Program.hpp"
#include "tools/gfx/opengl/Texture2D.hpp"
#include "tools/gfx/opengl/VertexBuffer.hpp"
#include "tools/gfx/GLRenderer.hpp"
#include "tools/logger/Logger.hpp"

#ifdef _MSC_VER
# pragma warning(disable: 4244)
#endif

namespace Tools { namespace Gfx {

    namespace {
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
        rs.target = 0;
        rs.renderToTexture = false;
        this->_PushState(rs);

        GLenum error = glewInit();
        if (error != GLEW_OK)
            throw std::runtime_error(ToString("glewInit() failed: ") + ToString(glewGetErrorString(error)));
        if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
            throw std::runtime_error("ARB_vertex_shader and ARB_fragment_shader are required");

        Tools::log << "Glew version: " << (char const*)glewGetString(GLEW_VERSION) << "\n";
        Tools::log << "OpenGL version: " << (char const*)glGetString(GL_VERSION) << "\n";

        GLCHECK(glEnable(GL_BLEND));
        //GLCHECK(glEnable(GL_ALPHA_TEST));
        GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        //GLCHECK(glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        InitDevIL();

        GLCHECK(glCullFace(GL_BACK));
    }

    void GLRenderer::Shutdown()
    {
        for (auto it = this->_shutdownCallbacks.begin(), ite = this->_shutdownCallbacks.end(); it != ite; ++it)
            (*it)();
    }

    std::unique_ptr<IVertexBuffer> GLRenderer::CreateVertexBuffer()
    {
        return std::unique_ptr<IVertexBuffer>(new OpenGL::VertexBuffer(*this));
    }

    std::unique_ptr<IIndexBuffer> GLRenderer::CreateIndexBuffer()
    {
        return std::unique_ptr<IIndexBuffer>(new OpenGL::IndexBuffer(*this));
    }

    std::unique_ptr<IRenderTarget> GLRenderer::CreateRenderTarget(glm::uvec2 const& imgSize)
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

    std::unique_ptr<IProgram> GLRenderer::CreateProgram(std::string const& vertex, std::string const& fragment)
    {
        return std::unique_ptr<IProgram>(new OpenGL::Program(*this, OpenGL::VertexProgram(vertex), OpenGL::FragmentProgram(fragment)));
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
        this->_PushState(rs);

        GLCHECK(glEnable(GL_DEPTH_TEST));
        GLCHECK(glEnable(GL_CULL_FACE));
    }

    void GLRenderer::EndDraw()
    {
        this->_PopState();
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

    // States
    void GLRenderer::SetScreenSize(glm::uvec2 const& size)
    {
        this->_screenSize = size;
        GLCHECK(::glViewport(0, 0, size.x, size.y));
    }

    void GLRenderer::SetViewport(glm::uvec2 const& offset, glm::uvec2 const& size)
    {
        GLCHECK(::glViewport(offset.x, offset.y, size.x, size.y));
    }

    void GLRenderer::SetClearColor(glm::vec4 const& color)
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

    void GLRenderer::SetCullMode(CullMode::Type type)
    {
        switch (type)
        {
        case CullMode::None: glDisable(GL_CULL_FACE); break;
        case CullMode::Clockwise:
        case CullMode::CounterClockwise:
            glEnable(GL_CULL_FACE);
            glFrontFace(type == CullMode::Clockwise ? GL_CW : GL_CCW);
            break;
        default: throw std::invalid_argument("unknown CullMode");
        }
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

    void GLRenderer::_PushState(ARenderer::RenderState const& state)
    {
        this->_GenericPushState(state);
    }

    void GLRenderer::_PopState()
    {
        auto rsOld = this->_states.front();

        if (this->_GenericPopState())
        {
            GLCHECK(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0));
            this->SetViewport(glm::uvec2(0, 0), this->_screenSize);
        }

        if (rsOld.state != this->_currentState->state)
        {
            if (this->_currentState->state == RenderState::Draw3D)
            {
                GLCHECK(glEnable(GL_DEPTH_TEST));
                GLCHECK(glEnable(GL_CULL_FACE));
            }
            else if (this->_currentState->state == RenderState::Draw2D)
            {
                GLCHECK(glDisable(GL_DEPTH_TEST));
                GLCHECK(glDisable(GL_CULL_FACE));
            }
        }
    }

}}
