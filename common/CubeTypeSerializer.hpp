#ifndef __COMMON_CUBETYPESERIALIZER_HPP__
#define __COMMON_CUBETYPESERIALIZER_HPP__

#include "tools/ByteArray.hpp"

#include "common/CubeType.hpp"

namespace Common {

    struct CubeTypeSerializer
    {
        static std::unique_ptr<CubeType> Read(Tools::ByteArray const& p)
        {
            std::unique_ptr<CubeType> ptr(new CubeType());

            p.Read(ptr->id);
            p.Read(ptr->name);
            p.Read(ptr->luaFile);
            p.Read(ptr->solid);
            p.Read(ptr->transparent);

            return ptr;
        }

        static void Write(CubeType const& cube, Tools::ByteArray& p)
        {
            p.Write(cube.id);
            p.Write(cube.name);
            p.Write(cube.luaFile);
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
