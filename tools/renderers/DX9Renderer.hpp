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
    private:
        enum DrawState
        {
            DrawNone = 0,
            Draw2D,
            Draw3D
        };

        enum
        {
            MaxVertexElements = 8
        };

    private:
        static const glm::mat4 _glToDirectX;

        glm::uvec2 _screenSize;
        bool _fullscreen;
        Rectangle _viewport;
        glm::detail::tmat4x4<float> _modelViewProjection;
        glm::detail::tmat4x4<float> _model;
        glm::detail::tmat4x4<float> _view;
        glm::detail::tmat4x4<float> _projection;
        unsigned int _currentMatrixMode;
        bool _useShaders;
        DrawState _state;

        LPDIRECT3D9 _object;
        LPDIRECT3DDEVICE9 _device;
        LPD3DXEFFECTPOOL _effectPool;
        std::list<DX9::ShaderProgram*> _allPrograms;
        std::list<DX9::RenderTarget*> _allRenderTargets;
        IDirect3DSurface9* _backBuffer;
        IDirect3DSurface9* _backZBuffer;
        bool _resetRenderTarget;

        IShaderProgram* _currentProgram;
        DX9::VertexBuffer* _vertexBuffer;

        Color4f _clearColor;
        float _clearDepth;
        int _clearStencil;

    public:
        DX9Renderer(glm::uvec2 const& screenSize, bool fullscreen) : _screenSize(screenSize), _fullscreen(fullscreen), _state(DrawNone), _object(0), _device(0), _backBuffer(0), _backZBuffer(0), _currentProgram(0) {}
        virtual ~DX9Renderer() { this->Shutdown(); }

        virtual std::string const& GetRendererName() const
        {
            static std::string s = "DirectX 9 Renderer (using Cg)";
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

        virtual void UpdateCurrentParameters();
        virtual void DrawElements(Uint32 count, Renderers::DataType::Type indicesType, void const* indices, Renderers::DrawingMode::Type mode = Renderers::DrawingMode::Triangles);
        virtual void DrawVertexBuffer(Uint32 offset, Uint32 count, Renderers::DrawingMode::Type mode = Renderers::DrawingMode::Triangles);

        // Matrices
        virtual void SetModelMatrix(glm::detail::tmat4x4<float> const& matrix);
        virtual void SetViewMatrix(glm::detail::tmat4x4<float> const& matrix);
        virtual void SetProjectionMatrix(glm::detail::tmat4x4<float> const& matrix);

        glm::detail::tmat4x4<float> const& GetModelViewProjectionMatrix() const { return this->_modelViewProjection; }
        glm::detail::tmat4x4<float> const& GetModelMatrix() const { return this->_model; }
        glm::detail::tmat4x4<float> const& GetViewMatrix() const { return this->_view; }
        glm::detail::tmat4x4<float> const& GetProjectionMatrix() const { return this->_projection; }

        // States
        virtual void SetScreenSize(glm::uvec2 const& size);
        virtual void SetClearColor(Color4f const& color) { this->_clearColor = color; }
        virtual void SetClearDepth(float value) { this->_clearDepth = value; }
        virtual void SetClearStencil(int value) { this->_clearStencil = value; }
        virtual void SetNormaliseNormals(bool normalise);
        virtual void SetDepthTest(bool enabled);
        virtual void SetDepthWrite(bool enabled);
        virtual void SetCullFace(bool enabled);
        virtual void SetViewport(Rectangle const& viewport);
        virtual void SetRasterizationMode(Renderers::RasterizationMode::Type rasterizationMode);

        IShaderProgram& GetCurrentProgram() { return *this->_currentProgram; }
        void SetCurrentProgram(IShaderProgram& program)
        {
            this->_currentProgram = &program;
            this->_modelViewProjection = this->_projection * this->_view * this->_model;
        }
        void SetVertexBuffer(DX9::VertexBuffer& vb) { this->_vertexBuffer = &vb; }
        LPDIRECT3DDEVICE9 GetDevice() const { return this->_device; }
        LPD3DXEFFECTPOOL GetEffectPool() const { return this->_effectPool; }
        void Present();
        void Unregister(DX9::ShaderProgram& program);
        void Unregister(DX9::RenderTarget& renderTarget);
        void _RefreshDevice();
    };

}}

#endif