#ifdef _WIN32
#pragma once // include guard non standard

#include "tools/renderers/dx9/directx.hpp"
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
        Vector2u _screenSize;
        Rectangle _viewport;
        Matrix4<float> _modelViewProjection;
        Matrix4<float> _model;
        Matrix4<float> _view;
        Matrix4<float> _projection;
        unsigned int _currentMatrixMode;
        bool _useShaders;
        DrawState _state;

        LPDIRECT3D9 _object;
        LPDIRECT3DDEVICE9 _device;
        LPD3DXEFFECTPOOL _effectPool;
        std::list<DX9::ShaderProgram*> _allPrograms;

        IShaderProgram* _currentProgram;
        DX9::VertexBuffer* _vertexBuffer;

        Color4f _clearColor;
        float _clearDepth;
        int _clearStencil;

    public:
        DX9Renderer(Vector2u const& screenSize) : _screenSize(screenSize), _state(DrawNone), _object(0), _device(0), _currentProgram(0) {}
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
        virtual std::unique_ptr<Renderers::ITexture2D> CreateTexture2D(Renderers::PixelFormat::Type format, Uint32 size, void const* data, Vector2u const& imgSize = Vector2u(0), void const* mipmapData = 0);
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
        virtual void SetModelMatrix(Matrix4<float> const& matrix);
        virtual void SetViewMatrix(Matrix4<float> const& matrix);
        virtual void SetProjectionMatrix(Matrix4<float> const& matrix);

        Matrix4<float> const& GetModelViewProjectionMatrix() const { return this->_modelViewProjection; }
        Matrix4<float> const& GetModelMatrix() const { return this->_model; }
        Matrix4<float> const& GetViewMatrix() const { return this->_view; }
        Matrix4<float> const& GetProjectionMatrix() const { return this->_projection; }

        // States
        virtual void SetScreenSize(Vector2u const& size);
        virtual void SetClearColor(Color4f const& color) { this->_clearColor = color; }
        virtual void SetClearDepth(float value) { this->_clearDepth = value; }
        virtual void SetClearStencil(int value) { this->_clearStencil = value; }
        virtual void SetNormaliseNormals(bool normalise);
        virtual void SetDepthTest(bool enabled);
        virtual void SetCullFace(bool enabled);
        virtual void SetViewport(Rectangle const& viewport);
        virtual void SetRasterizationMode(Renderers::RasterizationMode::Type rasterizationMode);

        IShaderProgram& GetCurrentProgram() { return *this->_currentProgram; }
        void SetCurrentProgram(IShaderProgram& program)
        {
            this->_currentProgram = &program;
            this->_modelViewProjection = this->_model * this->_view * this->_projection;
        }
        void SetVertexBuffer(DX9::VertexBuffer& vb) { this->_vertexBuffer = &vb; }
        LPDIRECT3DDEVICE9 GetDevice() const { return this->_device; }
        LPD3DXEFFECTPOOL GetEffectPool() const { return this->_effectPool; }
        void Present();
        void RegisterProgram(DX9::ShaderProgram& program);
        void UnregisterProgram(DX9::ShaderProgram& program);
    };

}}

#endif