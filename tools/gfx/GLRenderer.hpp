#pragma once

#include "tools/gfx/opengl/opengl.hpp"
#include "tools/gfx/ARenderer.hpp"
#include "tools/gfx/IRenderer.hpp"
#include "tools/Rectangle.hpp"
#include "tools/Vector2.hpp"

namespace Tools { namespace Gui {
    class WindowImplem;
}}

namespace Tools { namespace Gfx {

    namespace OpenGL {
        class IndexBuffer;
        class ShaderProgram;
        class VertexBuffer;
    }

    class GLRenderer : public ARenderer
    {
    public:
        OpenGL::VertexBuffer* bindedVertexBuffer;
        OpenGL::IndexBuffer* bindedIndexBuffer;

    public:
        GLRenderer(glm::uvec2 const& screenSize, bool fullscreen) : ARenderer(screenSize, fullscreen), bindedVertexBuffer(0), bindedIndexBuffer(0) {}
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
        virtual std::unique_ptr<IRenderTarget> CreateRenderTarget(glm::uvec2 const& imgSize);
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
        virtual void SetDepthTest(bool enabled);
        virtual void SetDepthWrite(bool enabled);
        virtual void SetCullMode(CullMode::Type type);
        virtual void SetRasterizationMode(RasterizationMode::Type rasterizationMode);
        void SetMatrixMode(unsigned int mode);

        // Misc
        virtual bool IsYTexCoordInverted() const { return this->_currentState->renderToTexture; }

    private:
        void _PushState(ARenderer::RenderState const& state);
        void _PopState();
    };

}}
