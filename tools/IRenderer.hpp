#ifndef __TOOLS_IRENDERER_HPP__
#define __TOOLS_IRENDERER_HPP__

#include "tools/Color.hpp"
#include "tools/Matrix4.hpp"
#include "tools/Rectangle.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

namespace Tools {

    namespace ClearFlags
    {
        enum Type
        {
            Color = 0x01,
            Depth = 0x02,
            Stencil = 0x04
        };
    }

    namespace Renderers
    {
        namespace DataType
        {
            /**
             * 0x<float><size><signed>
             */
            enum Type
            {
                Byte = 0x000101,
                UnsignedByte = 0x000100,
                Short = 0x000201,
                UnsignedShort = 0x000200,
                Int = 0x000401,
                UnsignedInt = 0x000400,
                Float = 0x010400
            };

            inline bool IsSigned(Type type) { return (type & 0x01) == 0x01; }
            inline std::size_t GetSize(Type type) { return (type & 0x00FF00) >> 8; }
            inline bool IsFloat(Type type) { return (type & 0x010000) == 0x010000; }
        }

        namespace PixelFormat
        {
            /**
             * 0x<size><type><nb color><format>
             * format: Alpha = 1, Rgb = 2, Rgba = 3, Luminance = 4, png = 5, LuminanceAlpha = 6
             * type: 1*uint8 = 1, 4*uint8 = 2, 1*float = 3, 4*uint4 = 4, 2*uint8 = 5
             */
            enum Type
            {
                Alpha8 = 0x01010101,
                Rgb8 = 0x03020302,
                Rgba8 = 0x04020403,
                Luminance = 0x04030104,
                Luminance8 = 0x01010104,
                Luminance8Alpha8 = 0x02050206,
                Rgb4 = 0x02040302,
                Rgba4 = 0x02040403,
                Png = 0x00000405
            };
        }

        namespace VertexBufferUsage
        {
            enum Type
            {
                Stream,
                Static,
                Dynamic
            };
        }

        namespace VertexAttributeUsage
        {
            enum Type
            {
                Position = 0,
                Color,
                Normal,
                TexCoord,
                Custom1,
                Custom2,
                Custom3,
                Custom4
            };
        }

        namespace DrawingMode
        {
            enum Type
            {
                Points,
                Lines,
                Triangles,
                TrianglesStrip,
                TrianglesFan
            };
        }

        namespace ShaderParameterUsage
        {
            enum Type
            {
                ModelViewProjectionMatrix,
                ModelViewMatrix,
                ModelMatrix,
                ViewMatrix,
                ProjectionMatrix
            };
        }

        namespace TextureFilter
        {
            /**
             * 0x<usable in far (min)><usable in near (mag)><id>
             */
            enum Type
            {
                Nearest = 0x1100,
                Linear = 0x1101,
                NearestMipmapNearest = 0x0102,
                NearestMipmapLinear = 0x0103,
                LinearMipmapLinear = 0x0104,
                LinearMipmapNearest = 0x0105,
            };
        }

        namespace RasterizationMode
        {
            enum Type
            {
                Point,
                Line,
                Fill
            };
        }

        class ITexture2D
        {
        public:
            virtual ~ITexture2D() {}
            virtual Vector2u const& GetSize() const = 0;
            virtual bool HasAlpha() const = 0;
            virtual void Bind() = 0;
            virtual void Unbind() = 0;

        protected:
            ITexture2D() {}
        private:
            ITexture2D(ITexture2D const&);
            ITexture2D& operator =(ITexture2D const&);
        };

        class IRenderTarget
        {
        public:
            virtual ~IRenderTarget() {}
            virtual ITexture2D& GetTexture() = 0;
            virtual void Bind() = 0;
        };

        class IIndexBuffer
        {
        public:
            virtual ~IIndexBuffer() {}
            virtual void SetData(Renderers::DataType::Type indicesType, std::size_t size, void const* data) = 0;
            virtual void SetSubData(std::size_t offset, std::size_t size, void const* data) = 0;
            virtual void Bind() = 0;
            virtual void Unbind() = 0;

        protected:
            IIndexBuffer() {}
        private:
            IIndexBuffer(IIndexBuffer const&);
            IIndexBuffer& operator =(IIndexBuffer const&);
        };

        class IVertexBuffer
        {
        public:
            virtual ~IVertexBuffer() {}
            virtual void PushVertexAttribute(DataType::Type type, VertexAttributeUsage::Type usage, Uint32 nbElements) = 0;
            virtual void SetData(std::size_t size, void const* data, VertexBufferUsage::Type usage) = 0;
            virtual void SetSubData(std::size_t offset, std::size_t size, void const* data) = 0;
            virtual void Bind() = 0;
            virtual void Unbind() = 0;

        protected:
            IVertexBuffer() {}
        private:
            IVertexBuffer(IVertexBuffer const&);
            IVertexBuffer& operator =(IVertexBuffer const&);
        };

        class IShaderParameter
        {
        public:
            virtual ~IShaderParameter() {}

            virtual void Set(bool value) = 0;
            virtual void Set(int value) = 0;
            virtual void Set(float value) = 0;
            virtual void Set(Color4<float> const& color) = 0;
            virtual void Set(Vector2<float> const& vector) = 0;
            virtual void Set(Vector3<float> const& vector) = 0;
            virtual void Set(Matrix4<float> const& matrix) = 0;
            virtual void Set(std::vector<glm::mat4x4> const& matrices) = 0;
            virtual void Set(ITexture2D& texture) = 0;

        protected:
            IShaderParameter() {}
        private:
            IShaderParameter(IShaderParameter const&);
            IShaderParameter& operator =(IShaderParameter const&);
        };

        class IShaderProgram
        {
        public:
            virtual ~IShaderProgram() {}

            virtual std::unique_ptr<IShaderParameter> GetParameter(std::string const& identifier) = 0;
            virtual std::unique_ptr<IShaderParameter> GetParameterFromSemantic(std::string const& semantic) = 0;
            virtual void SetParameterUsage(std::string const& identifier, ShaderParameterUsage::Type usage) = 0;
            virtual void UpdateParameter(ShaderParameterUsage::Type usage) = 0;
            virtual void UpdateCurrentPass() = 0;
            virtual void BeginPass() = 0;
            virtual bool EndPass() = 0; // Retourne true s'il n'y a plus de pass a faire sinon false.

        protected:
            IShaderProgram() {}
        private:
            IShaderProgram(IShaderProgram const&);
            IShaderProgram& operator =(IShaderProgram const&);
        };
    }

    class IRenderer
    {
    public:
        virtual ~IRenderer() {}

        virtual std::string const& GetRendererName() const = 0;

        virtual void Initialise() = 0;
        virtual void Shutdown() = 0;

        // Resources
        virtual std::unique_ptr<Renderers::IIndexBuffer> CreateIndexBuffer() = 0;
        virtual std::unique_ptr<Renderers::IVertexBuffer> CreateVertexBuffer() = 0;
        virtual std::unique_ptr<Renderers::IRenderTarget> CreateRenderTarget(Vector2u const& imgSize = Vector2u(0)) = 0;
        virtual std::unique_ptr<Renderers::ITexture2D> CreateTexture2D(Renderers::PixelFormat::Type format, Uint32 size, void const* data, Vector2u const& imgSize = Vector2u(0), void const* mipmapData = 0) = 0;
        virtual std::unique_ptr<Renderers::ITexture2D> CreateTexture2D(std::string const& imagePath) = 0;
        virtual std::unique_ptr<Renderers::IShaderProgram> CreateProgram(std::string const& effect) = 0;

        // Drawing
        virtual void Clear(int clearFlags) = 0;
        virtual void BeginDraw2D(Renderers::IRenderTarget* target = 0) = 0;
        virtual void EndDraw2D() = 0;
        virtual void BeginDraw(Renderers::IRenderTarget* target = 0) = 0;
        virtual void EndDraw() = 0;

        virtual void UpdateCurrentParameters() = 0;
        virtual void DrawElements(Uint32 count, Renderers::DataType::Type indicesType = Renderers::DataType::UnsignedInt, void const* indices = 0, Renderers::DrawingMode::Type mode = Renderers::DrawingMode::Triangles) = 0;
        virtual void DrawVertexBuffer(Uint32 offset, Uint32 count, Renderers::DrawingMode::Type mode = Renderers::DrawingMode::Triangles) = 0;

        // Matrices
        virtual void SetModelMatrix(Matrix4<float> const& matrix) = 0;
        virtual void SetViewMatrix(Matrix4<float> const& matrix) = 0;
        virtual void SetProjectionMatrix(Matrix4<float> const& matrix) = 0;

        // States
        virtual void SetScreenSize(Vector2u const& size) = 0;
        virtual void SetClearColor(Color4f const& color) = 0;
        virtual void SetClearDepth(float value) = 0;
        virtual void SetClearStencil(int value) = 0;
        virtual void SetNormaliseNormals(bool normalise) = 0;
        virtual void SetDepthTest(bool enabled) = 0;
        virtual void SetCullFace(bool enabled) = 0;
        virtual void SetViewport(Rectangle const& viewport) = 0;
        virtual void SetRasterizationMode(Renderers::RasterizationMode::Type rasterizationMode) = 0;

    protected:
        IRenderer() {}

    private:
        IRenderer(IRenderer const&);
        IRenderer& operator =(IRenderer const&);
    };
}

#endif
