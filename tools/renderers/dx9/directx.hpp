#pragma once

#include <Cg/cg.h>
#include <Cg/cgD3D9.h>

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    //inline GLint GetInternalFormatFromPixelFormat(Renderers::PixelFormat::Type format)
    //{
    //    return (format & 0x00FF00) >> 8;
    //}

    //inline GLint GetFormatFromPixelFormat(PixelFormat::Type format)
    //{
    //    switch (format & 0x0000FF)
    //    {
    //    case 1: return GL_ALPHA;
    //    case 2: return GL_RGB;
    //    case 3: return GL_RGBA;
    //    case 4: return GL_LUMINANCE;
    //    }
    //    throw std::runtime_error("Bad PixelFormat ?!");
    //}

    //inline GLint GetTypeFromPixelFormat(PixelFormat::Type format)
    //{
    //    switch ((format & 0xFF0000) >> 16)
    //    {
    //    case 1: return GL_UNSIGNED_BYTE;
    //    case 2: return GL_UNSIGNED_INT_8_8_8_8;
    //    case 3: return GL_UNSIGNED_SHORT_4_4_4_4;
    //    }
    //    throw std::runtime_error("Bad PixelFormat ?!");
    //}

    //inline GLint GetTypeFromDataType(DataType::Type type)
    //{
    //    switch (type)
    //    {
    //    case DataType::Byte: return GL_BYTE;
    //    case DataType::UnsignedByte: return GL_UNSIGNED_BYTE;
    //    case DataType::Short: return GL_SHORT;
    //    case DataType::UnsignedShort: return GL_UNSIGNED_SHORT;
    //    case DataType::Int: return GL_INT;
    //    case DataType::UnsignedInt: return GL_UNSIGNED_INT;
    //    case DataType::Float: return GL_FLOAT;
    //    }
    //    throw std::runtime_error("Bad DataType ?!");
    //}

    //inline GLint GetVertexBufferUsage(VertexBufferUsage::Type type)
    //{
    //    switch (type)
    //    {
    //    case VertexBufferUsage::Stream: return GL_STREAM_DRAW;
    //    case VertexBufferUsage::Static: return GL_STATIC_DRAW;
    //    case VertexBufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
    //    }
    //    throw std::runtime_error("Bad VertexBufferUsage ?!");
    //}

    inline D3DPRIMITIVETYPE GetDrawingMode(DrawingMode::Type type)
    {
        switch (type)
        {
        case DrawingMode::Points: return D3DPT_POINTLIST;
        case DrawingMode::Lines: return D3DPT_LINELIST;
        case DrawingMode::Triangles: return D3DPT_TRIANGLELIST;
        case DrawingMode::TrianglesStrip: return D3DPT_TRIANGLESTRIP;
        case DrawingMode::TrianglesFan: return D3DPT_TRIANGLEFAN;
        }
        throw std::runtime_error("Bad DrawingMode ?!");
    }

    inline UINT GetPrimitiveCount(DrawingMode::Type type, UINT vertices)
    {
        switch (type)
        {
        case DrawingMode::Points: return vertices;
        case DrawingMode::Lines: return vertices / 2;
        case DrawingMode::Triangles: return vertices / 3;
        case DrawingMode::TrianglesStrip: return vertices - 2;
        case DrawingMode::TrianglesFan: return vertices - 2;
        }
        throw std::runtime_error("Bad DrawingMode ?!");
    }

    //inline GLint GetTextureFilter(TextureFilter::Type type)
    //{
    //    switch (type)
    //    {
    //    case TextureFilter::Nearest: return GL_NEAREST;
    //    case TextureFilter::Linear: return GL_LINEAR;
    //    case TextureFilter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
    //    case TextureFilter::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
    //    case TextureFilter::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
    //    case TextureFilter::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
    //    }
    //    throw std::runtime_error("Bad TextureFilter ?!");
    //}

    //void GLCheckError(std::string const& file, unsigned int line, char const* function);

}}}
