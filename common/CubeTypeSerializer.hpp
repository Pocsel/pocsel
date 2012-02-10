#ifndef __COMMON_CUBETYPESERIALIZER_HPP__
#define __COMMON_CUBETYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"

#include "common/CubeType.hpp"

namespace Common {

    struct CubeTypeSerializer
    {
        static std::unique_ptr<CubeType> Read(Tools::ByteArray const& p)
        {
            BaseChunk::CubeType id;
            std::string name;
            p.Read(id);
            p.Read(name);

            std::unique_ptr<CubeType> ptr(new CubeType(id, name));
            p.Read(ptr->textures.top);
            p.Read(ptr->textures.left);
            p.Read(ptr->textures.front);
            p.Read(ptr->textures.right);
            p.Read(ptr->textures.back);
            p.Read(ptr->textures.bottom);

            p.Read(ptr->solid);
            p.Read(ptr->transparent);

            return ptr;
        }

        static void Write(CubeType const& cube, Tools::ByteArray& p)
        {
            p.Write(cube.id);
            p.Write(cube.name);
            p.Write(cube.textures.top);
            p.Write(cube.textures.left);
            p.Write(cube.textures.front);
            p.Write(cube.textures.right);
            p.Write(cube.textures.back);
            p.Write(cube.textures.bottom);

            p.Write(cube.solid);
            p.Write(cube.transparent);
        }

    private:
        static void Read(Tools::ByteArray const& p, CubeType&);
    };

}


namespace Tools {

    template<> struct ByteArray::Serializer< ::Common::CubeType > :
        public ::Common::CubeTypeSerializer
    {
    };

}

#endif
