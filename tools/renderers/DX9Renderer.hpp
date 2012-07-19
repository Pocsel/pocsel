#ifdef _WIN32
#pragma once // include guard non standard

#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/RenderTarget.hpp"
#include "tools/renderers/dx9/ShaderProgram.hpp"
#include "tools/renderers/dx9/VertexBuffer.hpp"

#include "tools/IRenderer.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

namespace Tools { namespace Renderers {

    class DX9Renderer : public IRenderer
    {
    public:
        enum
        {
            MaxVertexElements = 8,
            MaxRenderTargets = 4,
        };
    private:
        struct RenderState
        {
            enum DrawState
            {
                None,
                Draw2D,
                Draw3D
            };

            DrawState state;
            IRenderTarget* target;

            glm::detail::tmat4x4<float> modelViewProjection;
            glm::detail::tmat4x4<float> model;
            glm::detail::tmat4x4<float> view;
            glm::detail::tmat4x4<float> projection;
        };

    public:
        static const glm::mat4 glToDirectX;

    private:
        glm::uvec2 _screenSize;
        bool _fullscreen;
        bool _useShaders;

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
        std::list<std::function<void()>> _shutdownCallbacks;

        std::list<RenderState> _states;
        RenderState* _currentState;

        IShaderProgram* _currentProgram;
        DX9::VertexBuffer* _vertexBuffer;

        glm::vec4 _clearColor;
        float _clearDepth;
        int _clearStencil;

    public:
        DX9Renderer(glm::uvec2 const& screenSize, bool fullscreen) : _screenSize(screenSize), _fullscreen(fullscreen), _object(0), _device(0), _deviceState(Ok), _currentProgram(0) {}
        virtual ~DX9Renderer() { this->Shutdown(); }

        virtual std::string const& GetRendererName() const
        {
            static std::string s = "DirectX 9 Renderer (using Cg)";
            return s;
        }

        virtual void Initialise();
        virtual void Shutdown();

        // Callbacks
        virtual void RegisterShutdownCallback(std::function<void()>&& callback) { this->_shutdownCallbacks.push_back(callback); }

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

        virtual void UpdateCurrentParameters();
        virtual void DrawElements(Uint32 count, Renderers::DataType::Type indicesType, void const* indices, Renderers::DrawingMode::Type mode = Renderers::DrawingMode::Triangles);
        virtual void DrawVertexBuffer(Uint32 offset, Uint32 count, Renderers::DrawingMode::Type mode = Renderers::DrawingMode::Triangles);

        // Matrices
        virtual void SetModelMatrix(glm::detail::tmat4x4<float> const& matrix);
        virtual void SetViewMatrix(glm::detail::tmat4x4<float> const& matrix);
        virtual void SetProjectionMatrix(glm::detail::tmat4x4<float> const& matrix);

        glm::detail::tmat4x4<float> const& GetModelViewProjectionMatrix() const { return this->_currentState->modelViewProjection; }
        glm::detail::tmat4x4<float> const& GetModelMatrix() const { return this->_currentState->model; }
        glm::detail::tmat4x4<float> const& GetViewMatrix() const { return this->_currentState->view; }
        glm::detail::tmat4x4<float> const& GetProjectionMatrix() const { return this->_currentState->projection; }

        // States
        virtual void SetScreenSize(glm::uvec2 const& size);
        virtual void SetClearColor(glm::vec4 const& color) { this->_clearColor = color; }
        virtual void SetClearDepth(float value) { this->_clearDepth = value; }
        virtual void SetClearStencil(int value) { this->_clearStencil = value; }
        virtual void SetNormaliseNormals(bool normalise);
        virtual void SetDepthTest(bool enabled);
        virtual void SetDepthWrite(bool enabled);
        virtual void SetCullMode(Renderers::CullMode::Type type);
        virtual void SetRasterizationMode(Renderers::RasterizationMode::Type rasterizationMode);

        IShaderProgram& GetCurrentProgram() { return *this->_currentProgram; }
        void SetCurrentProgram(IShaderProgram& program)
        {
            this->_currentProgram = &program;
        }
        void SetVertexBuffer(DX9::VertexBuffer& vb) { this->_vertexBuffer = &vb; }
        LPDIRECT3DDEVICE9 GetDevice() const { return this->_device; }
        LPD3DXEFFECTPOOL GetEffectPool() const { return this->_effectPool; }
        void Present();
        void Unregister(DX9::ShaderProgram& program);
        void Unregister(DX9::RenderTarget& renderTarget);
    private:
        void _RefreshDevice(bool force = false);
        void _PushState(RenderState const& state);
        void _PopState();
    };

}}

#endif