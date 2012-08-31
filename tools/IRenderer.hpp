#ifndef __TOOLS_IRENDERER_HPP__
#define __TOOLS_IRENDERER_HPP__

#ifdef _MSC_VER
#include "tools/precompiled.hpp"
#endif

#include "tools/Rectangle.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

#ifdef None
# undef None
#endif

namespace Tools {
    class ByteArray;
}

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
             * format: Alpha = 1, Rgb = 2, Rgba = 3, Luminance = 4, png = 5, LuminanceAlpha = 6, DepthStencil = 7, Rg = 8, R = 9, Depth = A
             * type: 1*uint8 = 1, 4*uint8 = 2, 1*float = 3, 4*uint4 = 4, 2*uint8 = 5, others = 6, 2*float = 7
             */
            enum Type
            {
                //Alpha8 = 0x01010101, // deprecated
                Rgb8 = 0x03020302,
                Rgba8 = 0x04020403,
                //Luminance = 0x04030104, // deprecated
                //Luminance8 = 0x01010104, // deprecated
                //Luminance8Alpha8 = 0x02050206, // deprecated
                Rgb4 = 0x02040302,
                Rgba4 = 0x02040403,
                Png = 0x00000405,
                // Special
                R32f = 0x04030109,
                Rg16f = 0x04070208,
                Rgba16f = 0x08060403,
                Rgb10a2 = 0x04060403,
                Depth24Stencil8 = 0x04060207,
                Depth32 = 0x0406010A,
            };
        }

        namespace RenderTargetUsage
        {
            enum Type
            {
                Color,
                Depth,
                Stencil,
                DepthStencil
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
                TexCoord0,
                TexCoord1,
                TexCoord2,
                TexCoord3,
                TexCoord4,
                // For arrays only
                Max
            };
            static char const* const typeToName[] =
                {
                    "position",
                    "color",
                    "normal",
                    "texCoord0",
                    "texCoord1",
                    "texCoord2",
                    "texCoord3",
                    "texCoord4",
                };
        }

        namespace CullMode
        {
            enum Type
            {
                None,
                Clockwise,
                CounterClockwise
            };
        };

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
                ViewProjectionMatrix,
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
            virtual glm::uvec2 const& GetSize() const = 0;
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
            virtual ITexture2D& GetTexture(int idx) = 0;
            virtual int PushRenderTarget(PixelFormat::Type format, RenderTargetUsage::Type usage) = 0; // return an index for GetTexture
            virtual void Bind() = 0;
            virtual void Resize(glm::uvec2 const& newSize) = 0; // slow !
            virtual glm::uvec2 const& GetSize() const = 0;
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
            virtual void Set(glm::vec2 const& vector) = 0;
            virtual void Set(glm::vec3 const& vector) = 0;
            virtual void Set(glm::vec4 const& vector) = 0;
            virtual void Set(glm::mat4 const& matrix, bool isProjection = false) = 0;
            virtual void Set(std::vector<glm::mat4x4> const& matrices) = 0;
            virtual void Set(ITexture2D& texture) = 0;

            virtual bool IsUseable() const = 0;

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

            virtual IShaderParameter& GetParameter(std::string const& identifier) = 0;
            virtual IShaderParameter& GetParameterFromSemantic(std::string const& semantic) = 0;
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

        // Callbacks
        virtual void RegisterShutdownCallback(std::function<void()>&& callback) = 0;

        // Resources
        virtual std::unique_ptr<Renderers::IIndexBuffer> CreateIndexBuffer() = 0;
        virtual std::unique_ptr<Renderers::IVertexBuffer> CreateVertexBuffer() = 0;
        virtual std::unique_ptr<Renderers::IRenderTarget> CreateRenderTarget(glm::uvec2 const& imgSize = glm::uvec2(0)) = 0;
        virtual std::unique_ptr<Renderers::ITexture2D> CreateTexture2D(Renderers::PixelFormat::Type format, Uint32 size, void const* data, glm::uvec2 const& imgSize = glm::uvec2(0), void const* mipmapData = 0) = 0;
        virtual std::unique_ptr<Renderers::ITexture2D> CreateTexture2D(std::string const& imagePath) = 0;
        virtual std::unique_ptr<Renderers::IShaderProgram> CreateProgram(std::string const& effect) = 0;
        virtual std::unique_ptr<Renderers::IShaderProgram> CreateProgram(ByteArray& stream) = 0;

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
        virtual void SetModelMatrix(glm::detail::tmat4x4<float> const& matrix) = 0;
        virtual void SetViewMatrix(glm::detail::tmat4x4<float> const& matrix) = 0;
        virtual void SetProjectionMatrix(glm::detail::tmat4x4<float> const& matrix) = 0;

        // States
        virtual void SetScreenSize(glm::uvec2 const& size) = 0;
        virtual void SetViewport(glm::uvec2 const& offset, glm::uvec2 const& size) = 0;
        virtual void SetClearColor(glm::vec4 const& color) = 0;
        virtual void SetClearDepth(float value) = 0;
        virtual void SetClearStencil(int value) = 0;
        virtual void SetNormaliseNormals(bool normalise) = 0;
        virtual void SetDepthTest(bool enabled) = 0;
        virtual void SetDepthWrite(bool enabled) = 0;
        virtual void SetCullMode(Renderers::CullMode::Type type) = 0;
        virtual void SetRasterizationMode(Renderers::RasterizationMode::Type rasterizationMode) = 0;

    protected:
        IRenderer() {}

    private:
        IRenderer(IRenderer const&);
        IRenderer& operator =(IRenderer const&);
    };
}

#endif
