#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/IndexBuffer.hpp"
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
            if (error == CG_NO_ERROR)
                return;
            if (error == CG_COMPILER_ERROR)
                Tools::error << "Cg compiler:\n" << cgGetLastListing(_cgGlobalContext) << "\n";
            throw std::runtime_error(
                "An internal Cg call failed (Code: "
                + ToString(error) + "): "
                + cgGetErrorString(cgGetError()));
        }
    }

    void GLRenderer::Initialise()
    {
        this->_currentMatrixMode = -1;
        GLenum error = glewInit();
        if (error != GLEW_OK)
            throw std::runtime_error(ToString("glewInit() failed: ") + ToString(glewGetErrorString(error)));
        if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
            this->_useShaders = false;

        Tools::log << "Glew version: " << (char const*)glewGetString(GLEW_VERSION) << "\n";
        Tools::log << "OpenGL version: " << (char const*)glGetString(GL_VERSION) << "\n";

        GLCHECK(glEnable(GL_BLEND));
        GLCHECK(glEnable(GL_ALPHA_TEST));
        GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

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
        return std::unique_ptr<IIndexBuffer>(new OpenGL::IndexBuffer());
    }

    std::unique_ptr<ITexture2D> GLRenderer::CreateTexture2D(PixelFormat::Type format, Uint32 size, void const* data, Vector2u const& imgSize)
    {
        return std::unique_ptr<ITexture2D>(new OpenGL::Texture2D(*this, format, size, data, imgSize));
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

    /*
    std::unique_ptr<IPixelBuffer> GLRenderer::CreatePixelBuffer() = 0;
    std::unique_ptr<IVertexBuffer> GLRenderer::CreateVertexBuffer() = 0;
    std::unique_ptr<IFont> GLRenderer::CreateFont() = 0;
    */

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
        assert(this->_state == 0 && "Operation invalide");
        this->_state = Draw2D;

        this->_model = Tools::Matrix4<float>::identity;
        this->_view = Tools::Matrix4<float>::CreateTranslation(0, 0, 1);
        this->_projection = Tools::Matrix4<float>::CreateOrthographic(
                0,
                this->_viewport.size.w,
                this->_viewport.size.h,
                0,
                -float(this->_viewport.size.w),
                float(this->_viewport.size.w));
        this->_modelViewProjection = this->_model * this->_view * this->_projection;

        GLCHECK(glDisable(GL_CULL_FACE));
    }

    void GLRenderer::EndDraw2D()
    {
        this->_state = DrawNone;
        this->_currentProgram = 0;
    }

    void GLRenderer::BeginDraw(IRenderTarget* target)
    {
        assert(this->_state == 0 && "Operation invalide");
        this->_state = Draw3D;

        GLCHECK(glEnable(GL_DEPTH_TEST));
        GLCHECK(glEnable(GL_CULL_FACE));
    }

    void GLRenderer::EndDraw()
    {
        GLCHECK(glDisable(GL_CULL_FACE));
        GLCHECK(glDisable(GL_DEPTH_TEST));

        this->_state = DrawNone;
        this->_currentProgram = 0;
    }

    void GLRenderer::DrawElements(Uint32 count, DataType::Type indicesType, void const* indices, DrawingMode::Type mode)
    {
        GLCHECK(::glDrawElements(OpenGL::GetDrawingMode(mode), count, OpenGL::GetTypeFromDataType(indicesType), indices));
    }

    void GLRenderer::DrawVertexBuffer(Uint32 offset, Uint32 count, DrawingMode::Type mode)
    {
        GLCHECK(::glDrawArrays(OpenGL::GetDrawingMode(mode), offset, count));
    }

    // Matrices
    void GLRenderer::SetModelMatrix(Matrix4<float> const& matrix)
    {
        this->_model = matrix;
        if (this->_currentProgram != 0)
        {
            this->_modelViewProjection = this->_model * this->_view * this->_projection;
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
        }
    }

    void GLRenderer::SetViewMatrix(Matrix4<float> const& matrix)
    {
        this->_view = matrix;
        if (this->_currentProgram != 0)
        {
            this->_modelViewProjection = this->_model * this->_view * this->_projection;
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ViewMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
        }
    }

    void GLRenderer::SetProjectionMatrix(Matrix4<float> const& matrix)
    {
        this->_projection = matrix;
        if (this->_currentProgram != 0)
        {
            this->_modelViewProjection = this->_model * this->_view * this->_projection;
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ProjectionMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
        }
    }

    // States
    void GLRenderer::SetScreenSize(Vector2u const& size)
    {
        this->_screenSize = size;
    }

    void GLRenderer::SetViewport(Rectangle const& viewport)
    {
        GLCHECK(::glViewport(viewport.pos.x, this->_screenSize.h - viewport.size.y - viewport.pos.y, viewport.size.x, viewport.size.y));
        this->_viewport = viewport;
        if (this->_state == Draw2D)
        {
            this->_projection = Tools::Matrix4<float>::CreateOrthographic(
                0,
                this->_viewport.size.w,
                this->_viewport.size.h,
                0,
                -float(this->_viewport.size.w),
                float(this->_viewport.size.w));
            this->_modelViewProjection = this->_model * this->_view * this->_projection;
        }
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
        if (this->_currentMatrixMode == mode)
            return;
        this->_currentMatrixMode = mode;
        GLCHECK(glMatrixMode(mode));
    }

}}
