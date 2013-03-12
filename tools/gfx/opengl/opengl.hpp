#pragma once

#ifdef _WIN32
# include <GL/glew.h>
# include <GL/wglew.h>
#else
# include <GL/glew.h>
# include <GL/glxew.h>
#endif

#ifdef DEBUG
// In debug mode, perform a test on every OpenGL call
# define GLCHECK(Func) ((Func), Tools::Gfx::OpenGL::GLCheckError(__FILE__, __LINE__, __FUNCTION__))
#else
// Else, we don't add any overhead
# define GLCHECK(Func) (Func)
#endif

#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    inline GLint GetInternalFormatFromPixelFormat(PixelFormat::Type format)
    {
        switch (format)
        {
        case PixelFormat::R32f: return GL_R32F;
        case PixelFormat::Rg16f: return GL_RG16F;
        case PixelFormat::Rgba16f: return GL_RGBA16F_ARB;
        case PixelFormat::Rgb10a2: return GL_RGB10_A2;
        case PixelFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
        case PixelFormat::Depth32: return GL_DEPTH_COMPONENT32;
        default: return (format >> 8) & 0x00FF;
        }
    }

    inline GLint GetFormatFromPixelFormat(PixelFormat::Type format)
    {
        switch (format)
        {
        case PixelFormat::Depth24Stencil8: return GL_DEPTH_STENCIL;
        case PixelFormat::Depth32: return GL_DEPTH_COMPONENT;
        default:
            switch (format & 0x0000FF)
            {
            case 1: return GL_ALPHA;
            case 2: return GL_RGB;
            case 3: return GL_RGBA;
            case 8: return GL_RG;
            case 9: return GL_RED;
            default:
                throw std::runtime_error("Bad PixelFormat ?!");
            }
            break;
        }
    }

    inline GLint GetTypeFromPixelFormat(PixelFormat::Type format)
    {
        switch (format)
        {
        case PixelFormat::R32f: return GL_FLOAT;
        case PixelFormat::Rg16f: return GL_FLOAT;
        case PixelFormat::Rgba16f: return GL_FLOAT;
        case PixelFormat::Rgb10a2: return GL_UNSIGNED_INT_10_10_10_2;
        case PixelFormat::Depth24Stencil8: return GL_UNSIGNED_INT_24_8;
        case PixelFormat::Depth32: return GL_FLOAT;
        default:
            switch ((format >> 16) & 0xFF)
            {
            case 1: return GL_UNSIGNED_BYTE;
            case 2: return GL_UNSIGNED_INT_8_8_8_8;
            case 3: return GL_FLOAT;
            case 4: return GL_UNSIGNED_SHORT_4_4_4_4;
            case 5: return GL_UNSIGNED_BYTE;
            default:
                throw std::runtime_error("Bad PixelFormat ?!");
            }
            break;
        }
    }

    inline GLint GetTypeFromDataType(DataType::Type type)
    {
        switch (type)
        {
        case DataType::Byte: return GL_BYTE;
        case DataType::UnsignedByte: return GL_UNSIGNED_BYTE;
        case DataType::Short: return GL_SHORT;
        case DataType::UnsignedShort: return GL_UNSIGNED_SHORT;
        case DataType::Int: return GL_INT;
        case DataType::UnsignedInt: return GL_UNSIGNED_INT;
        case DataType::Float: return GL_FLOAT;
        default:
            throw std::runtime_error("Bad DataType ?!");
        }
    }

    inline GLint GetVertexBufferUsage(VertexBufferUsage::Type type)
    {
        switch (type)
        {
        case VertexBufferUsage::Stream: return GL_STREAM_DRAW;
        case VertexBufferUsage::Static: return GL_STATIC_DRAW;
        case VertexBufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        default:
            throw std::runtime_error("Bad VertexBufferUsage ?!");
        }
    }

    inline GLint GetDrawingMode(DrawingMode::Type type)
    {
        switch (type)
        {
        case DrawingMode::Points: return GL_POINTS;
        case DrawingMode::Lines: return GL_LINES;
        case DrawingMode::Triangles: return GL_TRIANGLES;
        case DrawingMode::TrianglesStrip: return GL_TRIANGLE_STRIP;
        case DrawingMode::TrianglesFan: return GL_TRIANGLE_FAN;
        default:
            throw std::runtime_error("Bad DrawingMode ?!");
        }
    }

    inline GLint GetTextureFilter(TextureFilter::Type type)
    {
        switch (type)
        {
        case TextureFilter::Nearest: return GL_NEAREST;
        case TextureFilter::Linear: return GL_LINEAR;
        case TextureFilter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
        case TextureFilter::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilter::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
        default:
            throw std::runtime_error("Bad TextureFilter ?!");
        }
    }

    inline GLint GetVertexAttributeIndex(VertexAttributeUsage::Type type)
    {
        switch (type)
        {
            case VertexAttributeUsage::Position: return 0;
            case VertexAttributeUsage::Normal: return 2;
            case VertexAttributeUsage::Color: return 3;
            case VertexAttributeUsage::TexCoord0: return 8;
            case VertexAttributeUsage::TexCoord1: return 9;
            case VertexAttributeUsage::TexCoord2: return 10;
            case VertexAttributeUsage::TexCoord3: return 11;
            case VertexAttributeUsage::TexCoord4: return 12;
            default:
                throw std::runtime_error("Bad VertexAttributeUsage ?!");
        }
    }

    inline char const* GetVertexAttributeSemantic(VertexAttributeUsage::Type type)
    {
        switch (type)
        {
            case VertexAttributeUsage::Position: return "POSITION";
            case VertexAttributeUsage::Normal: return "NORMAL";
            case VertexAttributeUsage::Color: return "COLOR";
            case VertexAttributeUsage::TexCoord0: return "TEXCOORD0";
            case VertexAttributeUsage::TexCoord1: return "TEXCOORD1";
            case VertexAttributeUsage::TexCoord2: return "TEXCOORD2";
            case VertexAttributeUsage::TexCoord3: return "TEXCOORD3";
            case VertexAttributeUsage::TexCoord4: return "TEXCOORD4";
            default:
                throw std::runtime_error("Bad VertexAttributeUsage ?!");
        }
    }

    void GLCheckError(std::string const& file, unsigned int line, char const* function);

}}}
