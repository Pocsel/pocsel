#ifndef __TOOLS_RENDERERS_GLRENDERER_HPP__
#define __TOOLS_RENDERERS_GLRENDERER_HPP__

#include "tools/IRenderer.hpp"
#include "tools/Color.hpp"
#include "tools/Rectangle.hpp"
#include "tools/Vector2.hpp"

namespace Tools { namespace Gui {
    class WindowImplem;
}}

namespace Tools { namespace Renderers {

    namespace OpenGL {
        class ShaderProgram;
    }

    class GLRenderer : public Tools::IRenderer
    {
    private:
        enum DrawState
        {
            DrawNone = 0,
            Draw2D,
            Draw3D
        };

    private:
        Vector2u _screenSize;
        Rectangle _viewport;
        Matrix4<float> _modelViewProjection;
        Matrix4<float> _model;
        Matrix4<float> _view;
        Matrix4<float> _projection;
        unsigned int _currentMatrixMode;

        DrawState _state;

        OpenGL::ShaderProgram* _currentProgram;

    public:
        GLRenderer() : _state(DrawNone), _currentProgram(0) {}
        virtual ~GLRenderer() { this->Shutdown(); }

        virtual std::string const& GetRendererName() const
        {
            static std::string s = "OpenGL Renderer (using GLEW)";
            return s;
        }

        virtual void Initialise();
        virtual void Shutdown();

        // Resources
        virtual std::unique_ptr<IVertexBuffer> CreateVertexBuffer();
        virtual std::unique_ptr<IIndexBuffer> CreateIndexBuffer();
        virtual std::unique_ptr<ITexture2D> CreateTexture2D(PixelFormat::Type format, Uint32 size, void const* data, Vector2u const& imgSize);
        virtual std::unique_ptr<ITexture2D> CreateTexture2D(std::string const& imagePath);
        virtual std::unique_ptr<IShaderProgram> CreateProgram(std::string const& vertexShader, std::string const& fragmentShader);

        // Drawing
        virtual void Clear(int clearFlags);
        virtual void BeginDraw2D(IRenderTarget* target = 0);
        virtual void EndDraw2D();
        virtual void BeginDraw(IRenderTarget* target);
        virtual void EndDraw();

        virtual void DrawElements(Uint32 count, DataType::Type indicesType, void const* indices, DrawingMode::Type mode = DrawingMode::Triangles);
        virtual void DrawVertexBuffer(Uint32 offset, Uint32 count, DrawingMode::Type mode = DrawingMode::Triangles);

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
        virtual void SetViewport(Rectangle const& viewport);
        virtual void SetClearColor(Color4f const& color);
        virtual void SetClearDepth(float value);
        virtual void SetClearStencil(int value);
        virtual void SetNormaliseNormals(bool normalise);
        virtual void SetDepthTest(bool enabled);
        virtual void SetCullFace(bool enabled);
        virtual void SetRasterizationMode(RasterizationMode::Type rasterizationMode);
        void SetMatrixMode(unsigned int mode);

        OpenGL::ShaderProgram& GetCurrentProgram() { return *this->_currentProgram; }
        void SetCurrentProgram(OpenGL::ShaderProgram& program)
        {
            this->_currentProgram = &program;
            this->_modelViewProjection = this->_model * this->_view * this->_projection;
        }
    };

}}

#endif
