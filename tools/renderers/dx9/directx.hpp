#pragma once

#include <Cg/cg.h>
#include <Cg/cgD3D9.h>

#include "DxErr.h"

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
    inline D3DFORMAT GetIndexBufferFormat(DataType::Type type)
    {
        switch (type)
        {
        case DataType::Short:
        case DataType::UnsignedShort:
            return D3DFMT_INDEX16;
        case DataType::Int:
        case DataType::UnsignedInt:
            return D3DFMT_INDEX32;
        case DataType::Byte:
        case DataType::UnsignedByte:
        case DataType::Float:
            break;
        }
        throw std::runtime_error("Bad DataType ?!");
    }

    inline D3DDECLTYPE GetTypeFromDataTypeAndNbElements(DataType::Type type, int nbElements)
    {
        switch (type)
        {
        case DataType::UnsignedByte:
            switch (nbElements)
            {
            case 4: return D3DDECLTYPE_UBYTE4;
            }
            break;
        case DataType::Short:
            switch (nbElements)
            {
            case 2: return D3DDECLTYPE_SHORT2;
            case 4: return D3DDECLTYPE_SHORT4;
            }
            break;
        case DataType::Float:
            switch (nbElements)
            {
            case 1: return D3DDECLTYPE_FLOAT1;
            case 2: return D3DDECLTYPE_FLOAT2;
            case 3: return D3DDECLTYPE_FLOAT3;
            case 4: return D3DDECLTYPE_FLOAT4;
            }
            break;

        case DataType::Byte:
        case DataType::UnsignedShort:
        case DataType::Int:
        case DataType::UnsignedInt:
            break;
        }
        throw std::runtime_error("Bad DataType ?!");
    }

    inline BYTE GetVertexAttributeUsage(VertexAttributeUsage::Type type)
    {
        switch (type)
        {
        case VertexAttributeUsage::Position: return D3DDECLUSAGE_POSITION;
        case VertexAttributeUsage::Normal: return D3DDECLUSAGE_NORMAL;
        case VertexAttributeUsage::Color: return D3DDECLUSAGE_COLOR;
        case VertexAttributeUsage::TexCoord:
        case VertexAttributeUsage::Custom1:
        case VertexAttributeUsage::Custom2:
        case VertexAttributeUsage::Custom3:
        case VertexAttributeUsage::Custom4:
            return D3DDECLUSAGE_TEXCOORD;
        }
        throw std::runtime_error("Bad VertexAttributeUsage ?!");
    }

    inline BYTE GetVertexAttributeUsageIndex(VertexAttributeUsage::Type type)
    {
        switch (type)
        {
        case VertexAttributeUsage::Position:
        case VertexAttributeUsage::Normal:
        case VertexAttributeUsage::Color:
        case VertexAttributeUsage::TexCoord:
            return 0;
        case VertexAttributeUsage::Custom1: return 1;
        case VertexAttributeUsage::Custom2: return 2;
        case VertexAttributeUsage::Custom3: return 3;
        case VertexAttributeUsage::Custom4: return 4;
        }
        throw std::runtime_error("Bad VertexAttributeUsage ?!");
    }

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

#define DXCHECKERROR(result) Tools::Renderers::DX9::_DXCHECKERROR(#result, result)

    inline void _DXCHECKERROR(const char* code, HRESULT result)
    {
        if (result < 0)
        {
            std::string err = DXGetErrorString(result);
            Tools::debug << "DirectX: " << code << ":\n" << err << std::endl;
            throw std::runtime_error("DirectX: " + err);
        }
    }

}}}
