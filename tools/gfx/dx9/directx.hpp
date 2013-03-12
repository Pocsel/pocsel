#pragma once

#ifdef DEBUG
# define D3D_DEBUG_INFO
#endif

#ifdef new
# undef new
#endif
#include <d3dx9.h>

#include "DxErr.h"
#ifdef DEBUG_NEW
# define new DEBUG_NEW
#endif

#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    struct ComPtrDeleter { void operator()(IUnknown* ptr) { ptr->Release(); } };
    template<class T>
    struct ComPtr
    {
    private:
        typedef std::unique_ptr<T, ComPtrDeleter> _base;
        _base _ptr;
    public:
        ComPtr() {}
        ComPtr(nullptr_t) {}
        explicit ComPtr(T* ptr) : _ptr(ptr) {}
        ComPtr(ComPtr&& ptr) : _ptr(std::move(ptr._ptr)) {}
        ComPtr& operator =(ComPtr&& ptr) { _ptr = std::move(ptr._ptr); return *this; }
        T* get() const { return _ptr.get(); }
        T& operator *() const { return _ptr.operator *(); }
        T* operator ->() const { return _ptr.operator ->(); }
        void reset(T* ptr = nullptr) { _ptr.reset(ptr); }
        bool operator ==(nullptr_t) { return _ptr == nullptr; }
        bool operator !=(nullptr_t) { return _ptr != nullptr; }
    private:
        ComPtr(ComPtr const&);
        ComPtr& operator =(ComPtr const&);
    };

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
        case VertexAttributeUsage::TexCoord0:
        case VertexAttributeUsage::TexCoord1:
        case VertexAttributeUsage::TexCoord2:
        case VertexAttributeUsage::TexCoord3:
        case VertexAttributeUsage::TexCoord4:
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
        case VertexAttributeUsage::TexCoord0: return 0;
        case VertexAttributeUsage::TexCoord1: return 1;
        case VertexAttributeUsage::TexCoord2: return 2;
        case VertexAttributeUsage::TexCoord3: return 3;
        case VertexAttributeUsage::TexCoord4: return 4;
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

#define DXCHECKERROR(result) Tools::Gfx::DX9::CheckError(result, #result)

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
        case PixelFormat::Rgb10a2: return D3DFMT_A2R10G10B10;
        case PixelFormat::Rgb8: return D3DFMT_R8G8B8;
        case PixelFormat::R32f: return D3DFMT_R32F;
        case PixelFormat::Rg16f: return D3DFMT_G16R16F;
        case PixelFormat::Rgba16f: return D3DFMT_A16B16G16R16F;
        case PixelFormat::Rgba4: return D3DFMT_A4R4G4B4;
        case PixelFormat::Rgba8: return D3DFMT_A8R8G8B8;
        case PixelFormat::Depth24Stencil8: return D3DFMT_D24S8;
        case PixelFormat::Depth32: return D3DFMT_D32;
        }
        throw std::runtime_error("Bad PixelFormat ?!");
    }

}}}
