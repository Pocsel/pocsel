#ifndef __CLIENT_RESOURCES_ITEXTURE_HPP__
#define __CLIENT_RESOURCES_ITEXTURE_HPP__

namespace Tools { namespace Renderers {
    class ITexture2D;
}}

namespace Client { namespace Resources {

    class ITexture
    {
    public:
        virtual ~ITexture() {}

        virtual void Update(Uint64 totalTime) = 0; // totalTime in microsecond
        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual Tools::Renderers::ITexture2D& GetCurrentTexture() = 0;
        virtual Tools::Renderers::ITexture2D const& GetCurrentTexture() const = 0;
        virtual bool HasAlpha() const = 0;
    };

}}

#endif
