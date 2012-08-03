#ifdef _WIN32

#include "tools/precompiled.hpp"

#include <IL/il.h>

#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/IndexBuffer.hpp"
#include "tools/renderers/dx9/RenderTarget.hpp"
#include "tools/renderers/dx9/ShaderProgram.hpp"
#include "tools/renderers/dx9/Texture2D.hpp"
#include "tools/renderers/dx9/VertexBuffer.hpp"
#include "tools/renderers/DX9Renderer.hpp"
#include "tools/logger/Logger.hpp"

#ifdef _MSC_VER
# pragma warning(disable: 4244)
#endif

namespace Tools { namespace Renderers {

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

    const glm::mat4 DX9Renderer::glToDirectX = glm::scale(1.0f, 1.0f, 0.5f) * glm::translate(0.0f, 0.0f, 1.0f);

    void DX9Renderer::Initialise()
    {
        this->_object = Direct3DCreate9(D3D_SDK_VERSION);
        if (this->_object == 0)
            throw std::runtime_error("DirectX: Could not create Direct3D Object");

        Tools::log << "Renderer: DirectX 9\n";

        D3DPRESENT_PARAMETERS present_parameters = {0};
        present_parameters.Windowed = !this->_fullscreen;
        present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        present_parameters.EnableAutoDepthStencil = true;
        present_parameters.AutoDepthStencilFormat = D3DFMT_D24S8;
        present_parameters.hDeviceWindow = GetActiveWindow();
        if (this->_fullscreen)
        {
            present_parameters.BackBufferWidth = this->_screenSize.x;
            present_parameters.BackBufferHeight = this->_screenSize.y;
        }
        present_parameters.BackBufferFormat = this->_fullscreen ? D3DFMT_X8R8G8B8 : D3DFMT_UNKNOWN;
        present_parameters.MultiSampleType = D3DMULTISAMPLE_NONE;

        DXCHECKERROR(this->_object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetActiveWindow(), D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE, &present_parameters, &this->_device));
        this->_presentParameters = present_parameters;

        DXCHECKERROR(this->_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
        DXCHECKERROR(this->_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE));

        DXCHECKERROR(this->_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
        DXCHECKERROR(this->_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
        DXCHECKERROR(this->_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD));

        RenderState rs;
        rs.state = RenderState::None;
        IDirect3DSurface9 *backColor, *backZBuffer;
        DXCHECKERROR(this->_device->GetRenderTarget(0, &backColor));
        DXCHECKERROR(this->_device->GetDepthStencilSurface(&backZBuffer));
        rs.target = new DX9::RenderTarget(*this, this->_screenSize, backColor, backZBuffer);
        this->_PushState(rs);

        D3DXCreateEffectPool(&this->_effectPool);

        InitDevIL();

        this->_clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        this->_clearDepth = 1.0f;
        this->_clearStencil = 0;
    }

    void DX9Renderer::Shutdown()
    {
        for (auto it = this->_shutdownCallbacks.begin(), ite = this->_shutdownCallbacks.end(); it != ite; ++it)
            (*it)();
        delete this->_states.back().target;
        this->_states.clear();
        if (this->_device != 0)
            this->_device->Release();
        if (this->_object != 0)
            this->_object->Release();
    }

    std::unique_ptr<IVertexBuffer> DX9Renderer::CreateVertexBuffer()
    {
        return std::unique_ptr<IVertexBuffer>(new DX9::VertexBuffer(*this));
    }

    std::unique_ptr<IIndexBuffer> DX9Renderer::CreateIndexBuffer()
    {
        return std::unique_ptr<IIndexBuffer>(new DX9::IndexBuffer(*this));
    }

    std::unique_ptr<Renderers::IRenderTarget> DX9Renderer::CreateRenderTarget(glm::uvec2 const& imgSize)
    {
        auto rt = new DX9::RenderTarget(*this, imgSize);
        this->_allRenderTargets.push_back(rt);
        return std::unique_ptr<IRenderTarget>(rt);
    }

    std::unique_ptr<ITexture2D> DX9Renderer::CreateTexture2D(PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize, void const* mipmapData)
    {
        return std::unique_ptr<ITexture2D>(new DX9::Texture2D(*this, format, size, data, imgSize, mipmapData));
    }

    std::unique_ptr<ITexture2D> DX9Renderer::CreateTexture2D(std::string const& imagePath)
    {
        return std::unique_ptr<ITexture2D>(new DX9::Texture2D(*this, imagePath));
    }

    std::unique_ptr<IShaderProgram> DX9Renderer::CreateProgram(std::string const& effect)
    {
        auto prog = new DX9::ShaderProgram(*this, effect);
        this->_allPrograms.push_back(prog);
        return std::unique_ptr<IShaderProgram>(prog);
    }

    // Drawing
    void DX9Renderer::Clear(int clearFlags)
    {
        DXCHECKERROR(this->_device->Clear(0, 0,
            (clearFlags & ClearFlags::Color ? D3DCLEAR_TARGET : 0) |
            (clearFlags & ClearFlags::Depth ? D3DCLEAR_ZBUFFER : 0) |
            (clearFlags & ClearFlags::Stencil ? D3DCLEAR_STENCIL : 0),
            D3DCOLOR_COLORVALUE(this->_clearColor.r, this->_clearColor.g, this->_clearColor.b, this->_clearColor.a), this->_clearDepth, this->_clearStencil));
    }

    void DX9Renderer::BeginDraw2D(IRenderTarget* target)
    {
        RenderState rs;
        rs.state = RenderState::Draw2D;
        rs.target = target;
        this->_PushState(rs);

        DXCHECKERROR(this->_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
        DXCHECKERROR(this->_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
    }

    void DX9Renderer::EndDraw2D()
    {
        this->_currentProgram = 0;
        this->_PopState();
    }

    void DX9Renderer::BeginDraw(IRenderTarget* target)
    {
        RenderState rs;
        rs.state = RenderState::Draw3D;
        rs.target = target;
        this->_PushState(rs);

        DXCHECKERROR(this->_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE));
        DXCHECKERROR(this->_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
    }

    void DX9Renderer::EndDraw()
    {
        this->_currentProgram = 0;
        this->_PopState();
    }

    void DX9Renderer::UpdateCurrentParameters()
    {
        assert(this->_currentProgram != 0 && "Il faut obligatoirement un shader !");
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelMatrix);
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ViewMatrix);
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ProjectionMatrix);
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewMatrix);
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ViewProjectionMatrix);
        this->_currentProgram->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
        this->_currentProgram->UpdateCurrentPass();
    }

    void DX9Renderer::DrawElements(Uint32 count, DataType::Type indicesType, void const* indices, DrawingMode::Type mode)
    {
        this->UpdateCurrentParameters();
        if (indices)
        {
            DX9::IndexBuffer ib(*this);
            ib.SetData(indicesType, count * DataType::GetSize(indicesType), indices);
            ib.Bind();
            DXCHECKERROR(this->_device->DrawIndexedPrimitive(DX9::GetDrawingMode(mode), 0, 0, this->_vertexBuffer->GetVerticesCount(), 0, DX9::GetPrimitiveCount(mode, count)));
            ib.Unbind();
        }
        else
            DXCHECKERROR(this->_device->DrawIndexedPrimitive(DX9::GetDrawingMode(mode), 0, 0, this->_vertexBuffer->GetVerticesCount(), 0, DX9::GetPrimitiveCount(mode, count)));
    }

    void DX9Renderer::DrawVertexBuffer(Uint32 offset, Uint32 count, DrawingMode::Type mode)
    {
        this->UpdateCurrentParameters();
        DXCHECKERROR(this->_device->DrawPrimitive(DX9::GetDrawingMode(mode), offset, DX9::GetPrimitiveCount(mode, count)));
    }

    // Matrices
    void DX9Renderer::SetModelMatrix(glm::detail::tmat4x4<float> const& matrix)
    {
        this->_currentState->model = matrix;
        this->_currentState->modelViewProjection = this->_currentState->projection * this->_currentState->view * this->_currentState->model;
    }

    void DX9Renderer::SetViewMatrix(glm::detail::tmat4x4<float> const& matrix)
    {
        this->_currentState->view = matrix;
        this->_currentState->modelViewProjection = this->_currentState->projection * this->_currentState->view * this->_currentState->model;
    }

    void DX9Renderer::SetProjectionMatrix(glm::detail::tmat4x4<float> const& matrix)
    {
        this->_currentState->projection = DX9Renderer::glToDirectX * matrix;
        this->_currentState->modelViewProjection = this->_currentState->projection * this->_currentState->view * this->_currentState->model;
    }

    // States
    void DX9Renderer::SetScreenSize(glm::uvec2 const& size)
    {
        this->_screenSize = size;
        this->_RefreshDevice(true);
    }

    void DX9Renderer::SetViewport(glm::uvec2 const& offset, glm::uvec2 const& size)
    {
        D3DVIEWPORT9 vp;
        vp.Width = size.x;
        vp.Height = size.y;
        vp.X = offset.x;
        vp.Y = offset.y;
        vp.MinZ = 0.0f;
        vp.MaxZ = 1.0f;
        DXCHECKERROR(this->_device->SetViewport(&vp));
    }

    void DX9Renderer::SetNormaliseNormals(bool normalise)
    {
        DXCHECKERROR(this->_device->SetRenderState(D3DRS_NORMALIZENORMALS, normalise ? TRUE : FALSE));
    }

    void DX9Renderer::SetDepthTest(bool enabled)
    {
        DXCHECKERROR(this->_device->SetRenderState(D3DRS_ZENABLE, enabled ? D3DZB_TRUE : D3DZB_FALSE));
    }

    void DX9Renderer::SetDepthWrite(bool enabled)
    {
        DXCHECKERROR(this->_device->SetRenderState(D3DRS_ZWRITEENABLE, enabled ? TRUE : FALSE));
    }

    void DX9Renderer::SetCullMode(Renderers::CullMode::Type type)
    {
        switch (type)
        {
        case Renderers::CullMode::None: this->_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); break;
        case Renderers::CullMode::Clockwise: this->_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); break;
        case Renderers::CullMode::CounterClockwise: this->_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); break;
        default: throw std::invalid_argument("unknown CullMode");
        }
    }

    void DX9Renderer::SetRasterizationMode(RasterizationMode::Type rasterizationMode)
    {
        switch (rasterizationMode)
        {
        case RasterizationMode::Point:
            DXCHECKERROR(this->_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT));
            break;

        case RasterizationMode::Line:
            DXCHECKERROR(this->_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME));
            break;

        case RasterizationMode::Fill:
            DXCHECKERROR(this->_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID));
            break;
        }
    }

    void DX9Renderer::Present()
    {
        HRESULT hr = this->_device->Present(0, 0, 0, 0);
        if (hr == D3DERR_DEVICELOST || hr == D3DERR_DEVICENOTRESET)
            this->_RefreshDevice();
        else
            DXCHECKERROR(hr);
    }

    void DX9Renderer::Unregister(DX9::RenderTarget& rt)
    {
        this->_allRenderTargets.remove(&rt);
    }

    void DX9Renderer::Unregister(DX9::ShaderProgram& program)
    {
        this->_allPrograms.remove(&program);
    }

    void DX9Renderer::_RefreshDevice(bool force)
    {
        bool invalidate = force || this->_deviceState == Ok;
        auto hr = this->_device->TestCooperativeLevel();
        if ((force && hr == S_OK) || hr == D3DERR_DEVICENOTRESET)
            this->_deviceState = NotReset;
        else
            this->_deviceState = hr != S_OK ? Lost : Ok;
        if (this->_deviceState == Ok)
            return;

        if (invalidate)
        {
            Tools::debug << "ScreenSize = " << ToString(this->_screenSize) << std::endl;

            // Release...
            auto& rs = this->_states.back();
            delete rs.target;
            for (auto it = this->_allPrograms.begin(), ite = this->_allPrograms.end(); it != ite; ++it)
                (*it)->GetEffect()->OnLostDevice();
            for (auto it = this->_allRenderTargets.begin(), ite = this->_allRenderTargets.end(); it != ite; ++it)
                (*it)->OnLostDevice();
        }

        if (this->_deviceState == NotReset)
        {
            this->_presentParameters.BackBufferWidth = this->_screenSize.x;
            this->_presentParameters.BackBufferHeight = this->_screenSize.y;
            DXCHECKERROR(this->_device->Reset(&this->_presentParameters));

            // Recreate...
            IDirect3DSurface9 *backColor, *backZBuffer;
            DXCHECKERROR(this->_device->GetRenderTarget(0, &backColor));
            DXCHECKERROR(this->_device->GetDepthStencilSurface(&backZBuffer));
            for (auto it = this->_allRenderTargets.begin(), ite = this->_allRenderTargets.end(); it != ite; ++it)
                (*it)->OnResetDevice();
            for (auto it = this->_allPrograms.begin(), ite = this->_allPrograms.end(); it != ite; ++it)
                (*it)->GetEffect()->OnResetDevice();
            this->_states.back().target = new DX9::RenderTarget(*this, this->_screenSize, backColor, backZBuffer);

            DXCHECKERROR(this->_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
            DXCHECKERROR(this->_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE));

            DXCHECKERROR(this->_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
            DXCHECKERROR(this->_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));
            DXCHECKERROR(this->_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD));

            this->_deviceState = Ok;
        }
    }

    void DX9Renderer::_PushState(DX9Renderer::RenderState const& state)
    {
        bool startScene = false;

        RenderState rsOld;
        if (this->_states.size() > 0)
        {
            rsOld = this->_states.front();
            if (state.target != 0)
                DXCHECKERROR(this->_device->EndScene());
            startScene = this->_states.size() == 1 || state.target != 0;
        }
        this->_states.push_front(state);
        auto& rs = this->_states.front();
        this->_currentState = &rs;

        if (rs.target != 0)
        {
            rs.target->Bind();
            D3DVIEWPORT9 vp;
            vp.Width = rs.target->GetSize().x;
            vp.Height = rs.target->GetSize().y;
            vp.X = 0;
            vp.Y = 0;
            vp.MinZ = 0.0f;
            vp.MaxZ = 1.0f;
            DXCHECKERROR(this->_device->SetViewport(&vp));
        }

        if (rs.state == DX9Renderer::RenderState::Draw2D)
        {
            rs.view = glm::translate<float>(0, 0, 1);
            auto size = rs.target == 0 ? this->_screenSize : rs.target->GetSize();
            rs.projection = DX9Renderer::glToDirectX * glm::ortho<float>(0, size.x, size.y, 0);
            rs.model = rsOld.model;
            rs.modelViewProjection = rs.projection * rs.view * rs.model;
        }
        else
        {
            rs.view = rsOld.view;
            rs.projection = rsOld.projection;
            rs.model = rsOld.model;
            rs.modelViewProjection = rs.projection * rs.view * rs.model;
        }

        if (startScene)
            DXCHECKERROR(this->_device->BeginScene());
    }

    void DX9Renderer::_PopState()
    {
        if (this->_states.size() <= 2)
            DXCHECKERROR(this->_device->EndScene());
        else if (this->_currentState->target != 0)
        {
            DXCHECKERROR(this->_device->EndScene());
            DXCHECKERROR(this->_device->BeginScene());
        }

        auto rsOld = this->_states.front();
        this->_states.pop_front();
        auto& rs = this->_states.front();
        this->_currentState = &rs;

        if (rsOld.target != 0)
        {
            if (rs.target != 0)
            {
                rs.target->Bind();
                D3DVIEWPORT9 vp;
                vp.Width = rs.target->GetSize().x;
                vp.Height = rs.target->GetSize().y;
                vp.X = 0;
                vp.Y = 0;
                vp.MinZ = 0.0f;
                vp.MaxZ = 1.0f;
                DXCHECKERROR(this->_device->SetViewport(&vp));
            }
            else
            {
                for (auto it = this->_states.begin(), ite = this->_states.end(); it != ite; ++it)
                {
                    if (it->target)
                    {
                        it->target->Bind();
                        D3DVIEWPORT9 vp;
                        vp.Width = it->target->GetSize().x;
                        vp.Height = it->target->GetSize().y;
                        vp.X = 0;
                        vp.Y = 0;
                        vp.MinZ = 0.0f;
                        vp.MaxZ = 1.0f;
                        DXCHECKERROR(this->_device->SetViewport(&vp));
                        break;
                    }
                }
            }
        }

        if (rsOld.state != rs.state)
        {
            if (rs.state == RenderState::Draw3D)
            {
                DXCHECKERROR(this->_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE));
                DXCHECKERROR(this->_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
            }
            else if (rs.state == RenderState::Draw2D)
            {
                DXCHECKERROR(this->_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
                DXCHECKERROR(this->_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
            }
        }
    }
}}

#endif
