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
        default:
            throw std::runtime_error("Bad DataType ?!");
        }
    }

    inline D3DDECLTYPE GetTypeFromDataTypeAndNbElements(DataType::Type type, int nbElements)
    {
        switch (type)
        {
        case DataType::UnsignedByte:
            switch (nbElements)
            {
            case 4: return D3DDECLTYPE_UBYTE4;
            default:
                throw std::runtime_error("Bad DataType ?!");
            }
            break;
        case DataType::Short:
            switch (nbElements)
            {
            case 2: return D3DDECLTYPE_SHORT2;
            case 4: return D3DDECLTYPE_SHORT4;
            default:
                throw std::runtime_error("Bad DataType ?!");
            }
            break;
        case DataType::Float:
            switch (nbElements)
            {
            case 1: return D3DDECLTYPE_FLOAT1;
            case 2: return D3DDECLTYPE_FLOAT2;
            case 3: return D3DDECLTYPE_FLOAT3;
            case 4: return D3DDECLTYPE_FLOAT4;
            default:
                throw std::runtime_error("Bad DataType ?!");
            }
            break;

        case DataType::Byte:
        case DataType::UnsignedShort:
        case DataType::Int:
        case DataType::UnsignedInt:
        default:
            throw std::runtime_error("Bad DataType ?!");
        }
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
        default:
            throw std::runtime_error("Bad VertexAttributeUsage ?!");
        }
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
        default:
            throw std::runtime_error("Bad VertexAttributeUsage ?!");
        }
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
        default:
            throw std::runtime_error("Bad DrawingMode ?!");
        }
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
        default:
            throw std::runtime_error("Bad DrawingMode ?!");
        }
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
        default:
            throw std::runtime_error("Bad PixelFormat ?!");
        }
    }

    inline DWORD GetTextureFilter(TextureFilter::Type filter)
    {
        switch (filter)
        {
        case TextureFilter::None: return D3DTEXF_NONE;
        case TextureFilter::Point: return D3DTEXF_POINT;
        case TextureFilter::Linear: return D3DTEXF_LINEAR;
        case TextureFilter::Anisotropic: return D3DTEXF_ANISOTROPIC;
        default:
            throw std::runtime_error("Bad TextureFilter");
        };
    }

    inline D3DCMPFUNC GetAlphaFunc(AlphaFunc::Type func)
    {
        switch (func)
        {
        case AlphaFunc::Never: return D3DCMP_NEVER;
        case AlphaFunc::Always: return D3DCMP_ALWAYS;
        case AlphaFunc::Equal: return D3DCMP_EQUAL;
        case AlphaFunc::Greater: return D3DCMP_GREATER;
        case AlphaFunc::GreaterEqual: return D3DCMP_GREATEREQUAL;
        case AlphaFunc::Less: return D3DCMP_LESS;
        case AlphaFunc::LessEqual: return D3DCMP_LESSEQUAL;
        case AlphaFunc::NotEqual: return D3DCMP_NOTEQUAL;
        default:
            throw std::runtime_error("Bad AlphaFunc");
        }
    }

    inline D3DBLEND GetBlend(Blend::Type blend)
    {
        switch (blend)
        {
        case Blend::Zero: return D3DBLEND_ZERO;
        case Blend::One: return D3DBLEND_ONE;
        case Blend::SrcColor: return D3DBLEND_SRCCOLOR;
        case Blend::InvSrcColor: return D3DBLEND_INVSRCCOLOR;
        case Blend::SrcAlpha: return D3DBLEND_SRCALPHA;
        case Blend::InvSrcAlpha: return D3DBLEND_INVSRCALPHA;
        case Blend::DestAlpha: return D3DBLEND_DESTALPHA;
        case Blend::InvDestAlpha: return D3DBLEND_INVDESTALPHA;
        case Blend::DestColor: return D3DBLEND_DESTCOLOR;
        case Blend::InvDestColor: return D3DBLEND_INVDESTCOLOR;
        case Blend::SrcAlphaSat: return D3DBLEND_SRCALPHASAT;
        case Blend::SrcColor2: return D3DBLEND_SRCCOLOR2;
        case Blend::InvSrcColor2: return D3DBLEND_INVSRCCOLOR2;
        default:
            throw std::runtime_error("Bad blend value");
        }
    }

    inline D3DBLENDOP GetBlendOp(BlendOp::Type op)
    {
        switch (op)
        {
        case BlendOp::Add: return D3DBLENDOP_ADD;
        case BlendOp::Subtract: return D3DBLENDOP_SUBTRACT;
        case BlendOp::RevSubtract: return D3DBLENDOP_REVSUBTRACT;
        case BlendOp::Min: return D3DBLENDOP_MIN;
        case BlendOp::Max: return D3DBLENDOP_MAX;
        default:
            throw std::runtime_error("Bad BlendOp");
        }
    }

    inline D3DCMPFUNC GetZFunc(ZFunc::Type func)
    {
        switch (func)
        {
        case ZFunc::Never: return D3DCMP_NEVER;
        case ZFunc::Always: return D3DCMP_ALWAYS;
        case ZFunc::Equal: return D3DCMP_EQUAL;
        case ZFunc::Greater: return D3DCMP_GREATER;
        case ZFunc::GreaterEqual: return D3DCMP_GREATEREQUAL;
        case ZFunc::Less: return D3DCMP_LESS;
        case ZFunc::LessEqual: return D3DCMP_LESSEQUAL;
        case ZFunc::NotEqual: return D3DCMP_NOTEQUAL;
        default:
            throw std::runtime_error("Bad ZFunc");
        }
    }

}}}
