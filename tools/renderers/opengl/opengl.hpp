#ifndef __TOOLS_OPENGL_HPP__
#define __TOOLS_OPENGL_HPP__

#include <stdexcept>

#ifdef _WIN32
# include <GL/glew.h>
# include <GL/wglew.h>
#else
# include <GL/glew.h>
# include <GL/glxew.h>
#endif

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#ifdef DEBUG
// In debug mode, perform a test on every OpenGL call
#define GLCHECK(Func) ((Func), Tools::Renderers::OpenGL::GLCheckError(__FILE__, __LINE__, __FUNCTION__))
#else
// Else, we don't add any overhead
#define GLCHECK(Func) (Func)
#endif

#include <string>

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    inline GLint GetInternalFormatFromPixelFormat(Renderers::PixelFormat::Type format)
    {
        if (format == PixelFormat::Luminance8Alpha8)
            return GL_LUMINANCE8_ALPHA8;
        return (format >> 8) & 0x00FF;
    }

    inline GLint GetFormatFromPixelFormat(PixelFormat::Type format)
    {
        switch (format & 0x0000FF)
        {
        case 1: return GL_ALPHA;
        case 2: return GL_RGB;
        case 3: return GL_RGBA;
        case 4: return GL_LUMINANCE;
        case 6: return GL_LUMINANCE_ALPHA;
        }
        throw std::runtime_error("Bad PixelFormat ?!");
    }

    inline GLint GetTypeFromPixelFormat(PixelFormat::Type format)
    {
        switch ((format >> 16) & 0xFF)
        {
        case 1: return GL_UNSIGNED_BYTE;
        case 2: return GL_UNSIGNED_INT_8_8_8_8;
        case 3: return GL_FLOAT;
        case 4: return GL_UNSIGNED_SHORT_4_4_4_4;
        case 5: return GL_UNSIGNED_BYTE;
        }
        throw std::runtime_error("Bad PixelFormat ?!");
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
        }
        throw std::runtime_error("Bad DataType ?!");
    }

    inline GLint GetVertexBufferUsage(VertexBufferUsage::Type type)
    {
        switch (type)
        {
        case VertexBufferUsage::Stream: return GL_STREAM_DRAW;
        case VertexBufferUsage::Static: return GL_STATIC_DRAW;
        case VertexBufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        }
        throw std::runtime_error("Bad VertexBufferUsage ?!");
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
        }
        throw std::runtime_error("Bad DrawingMode ?!");
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
        }
        throw std::runtime_error("Bad TextureFilter ?!");
    }

    void GLCheckError(std::string const& file, unsigned int line, char const* function);
    void CGCheckError(CGcontext ctx, std::string const& file, unsigned int line, char const* function);
}}}

#endif
