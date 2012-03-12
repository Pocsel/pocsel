#ifdef _WIN32

#include "tools/precompiled.hpp"

#include <IL/il.h>

#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/IndexBuffer.hpp"
#include "tools/renderers/dx9/ShaderProgramCg.hpp"
#include "tools/renderers/dx9/Texture2D.hpp"
#include "tools/renderers/dx9/VertexBuffer.hpp"
#include "tools/renderers/DX9Renderer.hpp"
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

    void DX9Renderer::Initialise()
    {
        this->_currentMatrixMode = -1;
        this->_object = Direct3DCreate9(D3D_SDK_VERSION);
        if (this->_object == 0)
            throw std::runtime_error("DirectX: Could not create Direct3D Object");

        Tools::log << "DirectX 9\n";

        D3DPRESENT_PARAMETERS present_parameters;
        std::memset(&present_parameters, 0, sizeof(present_parameters));
        present_parameters.Windowed = false;
        present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        present_parameters.EnableAutoDepthStencil = true;
        present_parameters.AutoDepthStencilFormat = D3DFMT_D16;
        present_parameters.hDeviceWindow = GetActiveWindow();
        present_parameters.BackBufferWidth = this->_screenSize.w;
        present_parameters.BackBufferHeight = this->_screenSize.h;
        present_parameters.BackBufferFormat = D3DFMT_A8B8G8R8;
        present_parameters.MultiSampleType = D3DMULTISAMPLE_NONE;

        if (FAILED(this->_object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetActiveWindow(), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present_parameters, &this->_device)))
            throw std::runtime_error("DirectX: Could not create Direct3D Device");

        if (_cgGlobalContext == 0)
            _cgGlobalContext = cgCreateContext();
        this->_cgContext = _cgGlobalContext;
        ++_cgGlobalNbReferences;
        cgSetErrorCallback(&ErrCallback);
        cgSetParameterSettingMode(this->_cgContext, CG_DEFERRED_PARAMETER_SETTING);
        cgD3D9SetDevice(this->_device);
        cgD3D9RegisterStates(this->_cgContext);
        cgD3D9SetManageTextureParameters(this->_cgContext, CG_TRUE);

        InitDevIL();
    }

    void DX9Renderer::Shutdown()
    {
        if (this->_device != 0)
            this->_device->Release();
        if (this->_object != 0)
            this->_object->Release();
        if (this->_useShaders && --_cgGlobalNbReferences == 0)
        {
            cgDestroyContext(this->_cgContext);
            _cgGlobalContext = 0;
        }
    }

    std::unique_ptr<IVertexBuffer> DX9Renderer::CreateVertexBuffer()
    {
        return std::unique_ptr<IVertexBuffer>(new DX9::VertexBuffer(*this));
    }

    std::unique_ptr<IIndexBuffer> DX9Renderer::CreateIndexBuffer()
    {
        return std::unique_ptr<IIndexBuffer>(new DX9::IndexBuffer(*this));
    }

    std::unique_ptr<ITexture2D> DX9Renderer::CreateTexture2D(PixelFormat::Type format, Uint32 size, void const* data, Vector2u const& imgSize, void const* mipmapData)
    {
        return std::unique_ptr<ITexture2D>(new DX9::Texture2D(*this, format, size, data, imgSize, mipmapData));
    }

    std::unique_ptr<ITexture2D> DX9Renderer::CreateTexture2D(std::string const& imagePath)
    {
        return std::unique_ptr<ITexture2D>(new DX9::Texture2D(*this, imagePath));
    }

    std::unique_ptr<IShaderProgram> DX9Renderer::CreateProgram(std::string const& effect)
    {
        return std::unique_ptr<IShaderProgram>(new DX9::ShaderProgramCg(*this, effect));
    }

    /*
    std::unique_ptr<IPixelBuffer> DX9Renderer::CreatePixelBuffer() = 0;
    std::unique_ptr<IVertexBuffer> DX9Renderer::CreateVertexBuffer() = 0;
    std::unique_ptr<IFont> DX9Renderer::CreateFont() = 0;
    */

    // Drawing
    void DX9Renderer::Clear(int clearFlags)
    {
        this->_device->Clear(0, 0,
            (clearFlags & ClearFlags::Color ? D3DCLEAR_TARGET : 0) |
            (clearFlags & ClearFlags::Depth ? D3DCLEAR_ZBUFFER : 0) |
            (clearFlags & ClearFlags::Stencil ? D3DCLEAR_STENCIL : 0),
            D3DCOLOR_COLORVALUE(this->_clearColor.r, this->_clearColor.g, this->_clearColor.b, this->_clearColor.a), this->_clearDepth, this->_clearStencil);
    }

    void DX9Renderer::BeginDraw2D(IRenderTarget* target)
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

        this->_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
        this->_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    }

    void DX9Renderer::EndDraw2D()
    {
        this->_state = DrawNone;
        this->_currentProgram = 0;
    }

    void DX9Renderer::BeginDraw(IRenderTarget* target)
    {
        assert(this->_state == 0 && "Operation invalide");
        this->_state = Draw3D;

        this->_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        this->_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    }

    void DX9Renderer::EndDraw()
    {
        this->_state = DrawNone;
        this->_currentProgram = 0;
    }

    void DX9Renderer::UpdateCurrentParameters()
    {
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelMatrix);
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewMatrix);
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
        if (this->_currentProgram != 0)
            this->_currentProgram->UpdateCurrentPass();
    }

    void DX9Renderer::DrawElements(Uint32 count, DataType::Type indicesType, void const* indices, DrawingMode::Type mode)
    {
        this->UpdateCurrentParameters();
        if (indices)
        {
            // TODO !
        }
        else
            this->_device->DrawIndexedPrimitive(DX9::GetDrawingMode(mode), 0, 0, count, 0, DX9::GetPrimitiveCount(mode, count));
        //GLCHECK(::glDrawElements(DX9::GetDrawingMode(mode), count, DX9::GetTypeFromDataType(indicesType), indices));
    }

    void DX9Renderer::DrawVertexBuffer(Uint32 offset, Uint32 count, DrawingMode::Type mode)
    {
        this->UpdateCurrentParameters();
        this->_device->DrawPrimitive(DX9::GetDrawingMode(mode), offset, count);
    }

    // Matrices
    void DX9Renderer::SetModelMatrix(Matrix4<float> const& matrix)
    {
        this->_model = matrix;
        if (this->_currentProgram != 0)
            this->_modelViewProjection = this->_model * this->_view * this->_projection;
    }

    void DX9Renderer::SetViewMatrix(Matrix4<float> const& matrix)
    {
        this->_view = matrix;
        if (this->_currentProgram != 0)
        {
            this->_modelViewProjection = this->_model * this->_view * this->_projection;
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ViewMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewMatrix);
            this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
        }
    }

    void DX9Renderer::SetProjectionMatrix(Matrix4<float> const& matrix)
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
    void DX9Renderer::SetScreenSize(Vector2u const& size)
    {
        this->_screenSize = size;
    }

    void DX9Renderer::SetViewport(Rectangle const& viewport)
    {
        D3DVIEWPORT9 vp;
        vp.Width = viewport.size.w;
        vp.Height = viewport.size.h;
        vp.X = viewport.pos.x;
        vp.Y = viewport.pos.y;
        vp.MinZ = 0.0f;
        vp.MaxZ = 1.0f;
        this->_device->SetViewport(&vp);
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

    void DX9Renderer::SetNormaliseNormals(bool normalise)
    {
        this->_device->SetRenderState(D3DRS_NORMALIZENORMALS, normalise ? TRUE : FALSE);
    }

    void DX9Renderer::SetDepthTest(bool enabled)
    {
        this->_device->SetRenderState(D3DRS_ZENABLE, enabled ? D3DZB_TRUE : D3DZB_FALSE);
    }

    void DX9Renderer::SetCullFace(bool enabled)
    {
        this->_device->SetRenderState(D3DRS_CULLMODE, enabled ? D3DCULL_CCW : D3DCULL_NONE);
    }

    void DX9Renderer::SetRasterizationMode(RasterizationMode::Type rasterizationMode)
    {
        switch (rasterizationMode)
        {
        case RasterizationMode::Point:
            this->_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
            break;

        case RasterizationMode::Line:
            this->_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
            break;

        case RasterizationMode::Fill:
            this->_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
            break;
        }
    }

    void DX9Renderer::Present()
    {
        this->_device->Present(0, 0, 0, 0);
    }

}}

#endif