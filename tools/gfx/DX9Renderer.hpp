#ifdef _WIN32
#pragma once

#include "tools/gfx/dx9/directx.hpp"
#include "tools/gfx/dx9/RenderTarget.hpp"
#include "tools/gfx/dx9/VertexBuffer.hpp"
#include "tools/gfx/ARenderer.hpp"

namespace Tools { namespace Gfx {

    namespace DX9 {
        class Program;
    }

    class DX9Renderer : public ARenderer
    {
    public:
        enum
        {
            MaxVertexElements = 8,
            MaxRenderTargets = 4,
        };

    public:
        static const glm::mat4 glToDirectX;

    private:
        LPDIRECT3D9 _object;
        LPDIRECT3DDEVICE9 _device;
        D3DPRESENT_PARAMETERS _presentParameters;
        enum
        {
            Ok,
            NotReset,
            Lost
        } _deviceState;
        LPD3DXEFFECTPOOL _effectPool;
        std::list<DX9::Program*> _allPrograms;
        std::list<DX9::RenderTarget*> _allRenderTargets;

        DX9::VertexBuffer* _vertexBuffer;

        glm::vec4 _clearColor;
        float _clearDepth;
        int _clearStencil;

    public:
        DX9Renderer(glm::uvec2 const& screenSize, bool fullscreen) : ARenderer(screenSize, fullscreen), _object(0), _device(0), _deviceState(Ok) {}
        virtual ~DX9Renderer() { this->Shutdown(); }

        virtual std::string const& GetRendererName() const
        {
            static std::string s = "DirectX 9 Renderer";
            return s;
        }

        virtual void Initialise();
        virtual void Shutdown();

        // Resources
        virtual std::unique_ptr<IIndexBuffer> CreateIndexBuffer();
        virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer();
        virtual std::unique_ptr<IRenderTarget> CreateRenderTarget(glm::uvec2 const& imgSize);
        virtual std::unique_ptr<ISamplerState> CreateSamplerState();
        virtual std::unique_ptr<ITexture2D> CreateTexture2D(PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize = glm::uvec2(0), void const* mipmapData = 0);
        virtual std::unique_ptr<ITexture2D> CreateTexture2D(std::string const& imagePath);
        virtual std::unique_ptr<IProgram> CreateProgram(std::string const& vertexCode, std::string const& fragmentCode);

        // Drawing
        virtual void Clear(int clearFlags);
        virtual void BeginDraw2D(IRenderTarget* target);
        virtual void EndDraw2D();
        virtual void BeginDraw(IRenderTarget* target);
        virtual void EndDraw();

        virtual void DrawElements(Uint32 count, DataType::Type indicesType, void const* indices, DrawingMode::Type mode = DrawingMode::Triangles);
        virtual void DrawVertexBuffer(Uint32 offset, Uint32 count, DrawingMode::Type mode = DrawingMode::Triangles);

        // Matrices
        virtual void SetProjectionMatrix(glm::detail::tmat4x4<float> const& matrix);

        // States
        virtual void SetScreenSize(glm::uvec2 const& size);
        virtual void SetViewport(glm::uvec2 const& offset, glm::uvec2 const& size);
        virtual void SetClearColor(glm::vec4 const& color) { this->_clearColor = color; }
        virtual void SetClearDepth(float value) { this->_clearDepth = value; }
        virtual void SetClearStencil(int value) { this->_clearStencil = value; }
        virtual void SetNormaliseNormals(bool normalise);
        virtual void SetDepthTest(bool enabled);
        virtual void SetDepthWrite(bool enabled);
        virtual void SetCullMode(CullMode::Type type);
        virtual void SetRasterizationMode(RasterizationMode::Type rasterizationMode);
        virtual bool IsYTexCoordInverted() const { return false; }

        virtual void SetAlphaBlendEnable(bool enabled);
        virtual void SetAlphaFunc(AlphaFunc::Type func);
        virtual void SetAlphaRef(float value);
        virtual void SetAlphaTestEnable(bool enabled);

        virtual void SetSrcBlend(Blend::Type blend);
        virtual void SetDestBlend(Blend::Type blend);
        virtual void SetBlendOp(BlendOp::Type op);
        virtual void SetSrcBlendAlpha(Blend::Type blend);
        virtual void SetDestBlendAlpha(Blend::Type blend);
        virtual void SetBlendOpAlpha(BlendOp::Type op);

        //virtual void SetColorWriteEnale(int colors);
        //virtual void SetDepthBias(float bias);

        virtual void SetDitherEnable(bool enabled);
        virtual void SetFillMode(FillMode::Type mode);

        virtual void SetZEnable(bool enabled);
        virtual void SetZFunc(ZFunc::Type func);
        virtual void SetZWriteEnable(bool enabled);


        void SetVertexBuffer(DX9::VertexBuffer& vb) { this->_vertexBuffer = &vb; }
        DX9::VertexBuffer* GetVertexBuffer() const { return this->_vertexBuffer; }
        LPDIRECT3DDEVICE9 GetDevice() const { return this->_device; }
        LPD3DXEFFECTPOOL GetEffectPool() const { return this->_effectPool; }
        void Present();
        void Unregister(DX9::Program& program);
        void Unregister(DX9::RenderTarget& renderTarget);

    private:
        void _RefreshDevice(bool force = false);
        void _PushState(ARenderer::RenderState const& state);
        void _PopState();
    };

}}

#endif