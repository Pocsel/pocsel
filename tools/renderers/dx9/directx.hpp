#pragma once

#ifdef DEBUG
# define D3D_DEBUG_INFO
#endif

#include <d3dx9.h>

#include "DxErr.h"

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

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

#define DXCHECKERROR(result) Tools::Renderers::DX9::CheckError(result, #result)

    inline HRESULT CheckError(HRESULT result, const char* code = 0)
    {
        if (FAILED(result))
        {
            std::string err = DXGetErrorString(result);
            Tools::debug << "DirectX: " << (code == 0 ? "" : code) << ":\n" << err << std::endl;
            throw std::runtime_error("DirectX: " + err);
        }
        return result;
    }

    inline D3DFORMAT GetFormat(PixelFormat::Type format)
    {
        switch (format)
        {
        case PixelFormat::Alpha8: return D3DFMT_A8;
        case PixelFormat::Luminance8: return D3DFMT_L8;
        case PixelFormat::Luminance8Alpha8: return D3DFMT_A8L8;
        case PixelFormat::Rgb10a2: return D3DFMT_A2R10G10B10;
        case PixelFormat::Rgb8: return D3DFMT_R8G8B8;
        case PixelFormat::Rgba16f: return D3DFMT_A16B16G16R16F;
        case PixelFormat::Rgba4: return D3DFMT_A4R4G4B4;
        case PixelFormat::Rgba8: return D3DFMT_A8R8G8B8;
        case PixelFormat::Depth24Stencil8: return D3DFMT_D24S8;
        }
        throw std::runtime_error("Bad PixelFormat ?!");
    }

}}}
