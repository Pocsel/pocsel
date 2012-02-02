#ifndef __COMMON_CUBETYPE_HPP__
#define __COMMON_CUBETYPE_HPP__

#include <string>

#include "common/BaseChunk.hpp"

namespace Common {

    struct CubeType
    {
        BaseChunk::CubeType id;
        std::string name;
        struct
        {
            Uint32 top;
            Uint32 left;
            Uint32 front;
            Uint32 right;
            Uint32 back;
            Uint32 bottom;
        } textures;

        bool solid;
        bool transparent;

        CubeType() {}
        CubeType(BaseChunk::CubeType id, std::string const& name) :
            id(id),
            name(name)
        {}

        // j'ai perdu du temps à cause de ces 2 chiasses useless !
        // ça avait l'air de servir a rien, alors j'ai commenté ! (david)
        // à moins que visual ne sache pas faire un constructeur
        // par copie tout seul ?
//        CubeType(CubeType const& descr) :
//            id(descr.id),
//            name(descr.name),
//            textures(descr.textures),
//            solid(solid),
//            transparent(transparent)
//        {}
//        CubeType(CubeType&& descr) :
//            id(descr.id),
//            name(descr.name),
//            textures(descr.textures),
//            solid(solid),
//            transparent(transparent)
//        {}
    };

}

#endif
