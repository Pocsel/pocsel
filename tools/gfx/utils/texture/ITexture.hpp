#pragma once

namespace Tools { namespace Gfx {
    class ITexture2D;
}}

namespace Tools { namespace Gfx { namespace Utils { namespace Texture {

    class ITexture
    {
    public:
        virtual ~ITexture() {}

        virtual ITexture* Clone() = 0;

        virtual void Update(Uint64 totalTime) = 0; // totalTime in microsecond
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual ITexture2D& GetCurrentTexture() = 0;
        virtual ITexture2D const& GetCurrentTexture() const = 0;
        virtual bool HasAlpha() const = 0;
    };

}}}}
