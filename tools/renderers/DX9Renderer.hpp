#ifdef _WIN32
#pragma once // include guard non standard

#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/RenderTarget.hpp"
#include "tools/renderers/dx9/ShaderProgram.hpp"
#include "tools/renderers/dx9/VertexBuffer.hpp"
#include "tools/renderers/ARenderer.hpp"

#include "tools/IRenderer.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

namespace Tools { namespace Renderers {

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
        std::list<DX9::ShaderProgram*> _allPrograms;
        std::list<DX9::RenderTarget*> _allRenderTargets;

        std::list<RenderState> _states;
        RenderState* _currentState;

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
        virtual std::unique_ptr<Renderers::IIndexBuffer> CreateIndexBuffer();
        virtual std::unique_ptr<Renderers::IVertexBuffer> CreateVertexBuffer();
        virtual std::unique_ptr<Renderers::IRenderTarget> CreateRenderTarget(glm::uvec2 const& imgSize);
        virtual std::unique_ptr<Renderers::ITexture2D> CreateTexture2D(Renderers::PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize = glm::uvec2(0), void const* mipmapData = 0);
        virtual std::unique_ptr<Renderers::ITexture2D> CreateTexture2D(std::string const& imagePath);
        virtual std::unique_ptr<Renderers::IShaderProgram> CreateProgram(std::string const& effect);

        // Drawing
        virtual void Clear(int clearFlags);
        virtual void BeginDraw2D(Renderers::IRenderTarget* target);
        virtual void EndDraw2D();
        virtual void BeginDraw(Renderers::IRenderTarget* target);
        virtual void EndDraw();

        virtual void DrawElements(Uint32 count, Renderers::DataType::Type indicesType, void const* indices, Renderers::DrawingMode::Type mode = Renderers::DrawingMode::Triangles);
        virtual void DrawVertexBuffer(Uint32 offset, Uint32 count, Renderers::DrawingMode::Type mode = Renderers::DrawingMode::Triangles);

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
        virtual void SetCullMode(Renderers::CullMode::Type type);
        virtual void SetRasterizationMode(Renderers::RasterizationMode::Type rasterizationMode);

        void SetVertexBuffer(DX9::VertexBuffer& vb) { this->_vertexBuffer = &vb; }
        DX9::VertexBuffer* GetVertexBuffer() const { return this->_vertexBuffer; }
        LPDIRECT3DDEVICE9 GetDevice() const { return this->_device; }
        LPD3DXEFFECTPOOL GetEffectPool() const { return this->_effectPool; }
        void Present();
        void Unregister(DX9::ShaderProgram& program);
        void Unregister(DX9::RenderTarget& renderTarget);
    private:
        void _RefreshDevice(bool force = false);
        void _PushState(ARenderer::RenderState const& state);
        void _PopState();
    };

}}

#endif