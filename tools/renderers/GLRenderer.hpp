#ifndef __TOOLS_RENDERERS_GLRENDERER_HPP__
#define __TOOLS_RENDERERS_GLRENDERER_HPP__

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/ARenderer.hpp"
#include "tools/IRenderer.hpp"
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
        class VertexBuffer;
    }

    class GLRenderer : public ARenderer
    {
    private:
        CGcontext _cgContext;

    public:
        OpenGL::VertexBuffer* bindedVertexBuffer;
        OpenGL::IndexBuffer* bindedIndexBuffer;

    public:
        GLRenderer(glm::uvec2 const& screenSize, bool fullscreen) : ARenderer(screenSize, fullscreen), _cgContext(0), bindedVertexBuffer(0), bindedIndexBuffer(0) {}
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
        virtual std::unique_ptr<IProgram> CreateProgram(std::string const& vertex, std::string const& fragment);

        // Drawing
        virtual void Clear(int clearFlags);
        virtual void BeginDraw2D(IRenderTarget* target);
        virtual void EndDraw2D();
        virtual void BeginDraw(IRenderTarget* target);
        virtual void EndDraw();

        virtual void DrawElements(Uint32 count, DataType::Type indicesType = DataType::UnsignedInt, void const* indices = 0, DrawingMode::Type mode = DrawingMode::Triangles);
        virtual void DrawVertexBuffer(Uint32 offset, Uint32 count, DrawingMode::Type mode = DrawingMode::Triangles);

        // States
        virtual void SetScreenSize(glm::uvec2 const& size);
        virtual void SetViewport(glm::uvec2 const& offset, glm::uvec2 const& size);
        virtual void SetClearColor(glm::vec4 const& color);
        virtual void SetClearDepth(float value);
        virtual void SetClearStencil(int value);
        virtual void SetNormaliseNormals(bool normalise);
        virtual void SetDepthTest(bool enabled);
        virtual void SetDepthWrite(bool enabled);
        virtual void SetCullMode(Renderers::CullMode::Type type);
        virtual void SetRasterizationMode(RasterizationMode::Type rasterizationMode);
        void SetMatrixMode(unsigned int mode);

        CGcontext GetCgContext() const { return this->_cgContext; }
    private:
        void _PushState(ARenderer::RenderState const& state);
        void _PopState();
    };

}}

#endif
