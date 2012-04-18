#ifndef __TOOLS_RENDERERS_GLRENDERER_HPP__
#define __TOOLS_RENDERERS_GLRENDERER_HPP__

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Color.hpp"
#include "tools/Rectangle.hpp"
#include "tools/Vector2.hpp"

// define fait dans X11
#ifdef None
#undef None
#endif

namespace Tools { namespace Gui {
    class WindowImplem;
}}

namespace Tools { namespace Renderers {

    namespace OpenGL {
        class IndexBuffer;
        class ShaderProgram;
    }

    class GLRenderer : public Tools::IRenderer
    {
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
            unsigned int matrixMode;
        };

    private:
        glm::uvec2 _screenSize;
        bool _useShaders;
        CGcontext _cgContext;

        std::list<RenderState> _states;
        RenderState* _currentState;

        IShaderProgram* _currentProgram;

    public:
        OpenGL::IndexBuffer* bindedIndexBuffer;

    public:
        GLRenderer(bool useShaders = true) : _useShaders(useShaders), _currentProgram(0), bindedIndexBuffer(0) {}
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
        virtual std::unique_ptr<Renderers::IRenderTarget> CreateRenderTarget(glm::uvec2 const& imgSize);
        virtual std::unique_ptr<ITexture2D> CreateTexture2D(PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize, void const* mipmapData);
        virtual std::unique_ptr<ITexture2D> CreateTexture2D(std::string const& imagePath);
        virtual std::unique_ptr<IShaderProgram> CreateProgram(std::string const& effect);

        // Drawing
        virtual void Clear(int clearFlags);
        virtual void BeginDraw2D(IRenderTarget* target);
        virtual void EndDraw2D();
        virtual void BeginDraw(IRenderTarget* target);
        virtual void EndDraw();

        virtual void UpdateCurrentParameters();
        virtual void DrawElements(Uint32 count, DataType::Type indicesType = DataType::UnsignedInt, void const* indices = 0, DrawingMode::Type mode = DrawingMode::Triangles);
        virtual void DrawVertexBuffer(Uint32 offset, Uint32 count, DrawingMode::Type mode = DrawingMode::Triangles);

        // Matrices
        virtual void SetModelMatrix(glm::detail::tmat4x4<float> const& matrix);
        virtual void SetViewMatrix(glm::detail::tmat4x4<float> const& matrix);
        virtual void SetProjectionMatrix(glm::detail::tmat4x4<float> const& matrix);

        glm::detail::tmat4x4<float> const& GetModelViewProjectionMatrix() const { return this->_states.front().modelViewProjection; }
        glm::detail::tmat4x4<float> const& GetModelMatrix() const { return this->_states.front().model; }
        glm::detail::tmat4x4<float> const& GetViewMatrix() const { return this->_states.front().view; }
        glm::detail::tmat4x4<float> const& GetProjectionMatrix() const { return this->_states.front().projection; }

        // States
        virtual void SetScreenSize(glm::uvec2 const& size);
        virtual void SetClearColor(Color4f const& color);
        virtual void SetClearDepth(float value);
        virtual void SetClearStencil(int value);
        virtual void SetNormaliseNormals(bool normalise);
        virtual void SetDepthTest(bool enabled);
        virtual void SetCullFace(bool enabled);
        virtual void SetRasterizationMode(RasterizationMode::Type rasterizationMode);
        void SetMatrixMode(unsigned int mode);

        IShaderProgram& GetCurrentProgram() { return *this->_currentProgram; }
        void SetCurrentProgram(IShaderProgram& program)
        {
            this->_currentProgram = &program;
        }
        CGcontext GetCgContext() const { return this->_cgContext; }
    private:
        void _PushState(RenderState const& state);
        void _PopState();
    };

}}

#endif
