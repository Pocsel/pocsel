#ifndef __TOOLS_MQM_HPP__
#define __TOOLS_MQM_HPP__

#include "tools/models/Iqm.hpp"

namespace Tools { namespace Models { namespace Mqm {

    static char const* const Magic = "biet QUAKEMODEL";
    static Uint32 const Version = 1;

    typedef Iqm::Header Header;
    typedef Iqm::Mesh Mesh;
    typedef Iqm::Triangle Triangle;
    typedef Iqm::Joint Joint;
    typedef Iqm::Pose Pose;
    typedef Iqm::Anim Anim;
    typedef Iqm::VertexArray VertexArray;

    namespace VertexArrayType {
        enum
        {
            Position     = 0,
            Texcoord     = 1,
            Normal       = 2,
            Tangent      = 3,
            BlendIndexes = 4,
            BlendWeights = 5,
            Color        = 6,
            Custom       = 0x10
        };
    }
    namespace VertexArrayFormat {
        enum
        {
            Byte   = 0,
            Ubyte  = 1,
            Short  = 2,
            Ushort = 3,
            Int    = 4,
            Uint   = 5,
            Half   = 6,
            Float  = 7,
            Double = 8,
        };
    }


}}}

#endif

