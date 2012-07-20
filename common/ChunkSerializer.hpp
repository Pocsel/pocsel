#ifndef __COMMON_CHUNKSERIALIZER_HPP__
#define __COMMON_CHUNKSERIALIZER_HPP__

#include "common/constants.hpp"
#include "tools/ByteArray.hpp"

/*
Uint64 Id
Uint8 Palette size
{
    0 = air only (chunk vide)
    1 = chunk plein d'un seul type
    2-16 = plusieurs trucs
    17+ = pas de palette
}
if (air only)
{
    c'est fini
}
if (chunk plein d'un seul type)
{
    ChunkType le type
    c'est fini
}
if (PALETTE)
{
    (ChunkType Type0, Type1, Type2, Type3...) * palette size (l'air est un type)
    while (size != 4096)
    {
        Uint16 MagicMask
        {
            4 bits de type
            12 bits de taille
        }
    }
}
if (pas de palette)
{
    while (size != 4096)
    {
        Uint8 taille
        ChunkType type
    }
}
*/

namespace Common {

    template<typename ChunkType>
    struct ChunkSerializer
    {
        typedef typename ChunkType::CubeType CubeType;
        typedef typename ChunkType::CubeArray CubeArray;

        static std::unique_ptr<ChunkType> Read(Tools::ByteArray const& p)
        {
            std::unique_ptr<ChunkType> chunkPtr(new ChunkType(p.Read64()));

            Uint8 number_of_types = p.Read8();


            if (number_of_types == 0) // empty chunk
            {
                return chunkPtr;
            }

            std::unique_ptr<CubeArray> cubesPtr(new CubeArray);
            CubeType* cubes = cubesPtr->data();

            if (number_of_types == 1)
            {
                static_assert(sizeof(CubeType) == 2, "faut changer le read16");
                CubeType type = p.Read16();
                for (unsigned int i = 0 ; i < Common::ChunkSize3 ; ++i)
                    cubes[i] = type;
            }
            else if (number_of_types <= 16)
            {
                CubeType types[16];
                for (Uint8 i = 0 ; i < number_of_types ; ++i)
                {
                    static_assert(sizeof(CubeType) == 2, "faut changer le read16");
                    types[i] = p.Read16();
                }
                unsigned int totalSize = 0;
                while (totalSize != Common::ChunkSize3)
                {
                    Uint16 size = p.Read16();
                    int typeIndex = (size & 0xF000) >> 12;
                    if (typeIndex >= number_of_types)
                        throw std::runtime_error("PROBLEM with chunk packet: a type with does no exist.");
                    CubeType type = types[typeIndex];
                    size &= 0x0FFF;
                    if (size + totalSize > Common::ChunkSize3)
                        throw std::runtime_error("PROBLEM with chunk packet: too many cubes in that chunk.");
                    for (unsigned int i = 0 ; i < size ; ++i)
                    {
                        cubes[totalSize + i] = type;
                    }
                    totalSize += size;
                }
            }
            else
            {
                unsigned int totalSize = 0;
                while (totalSize != Common::ChunkSize3)
                {
                    Uint8 size = p.Read8();
                    if (size + totalSize > Common::ChunkSize3)
                        throw std::runtime_error("PROBLEM with chunk packet: too many cubes in that chunk.");
                    static_assert(sizeof(CubeType) == 2, "faut changer le read16");
                    CubeType type = p.Read16();

                    for (unsigned int i = 0 ; i < size ; ++i)
                    {
                        cubes[totalSize + i] = type;
                    }
                    totalSize += size;
                }
            }

            chunkPtr->SetCubes(std::move(cubesPtr));
            return chunkPtr;
        }

        static void Write(ChunkType const& chunk, Tools::ByteArray& p)
        {
            p.Write64(chunk.id);
            if (chunk.IsEmpty())
            {
                p.Write8(0);
                return;
            }

            static_assert(sizeof(CubeType) < 3, "il faut changer la maniere de faire, sinon eclatage de pile");
            Uint8 types[1 << (sizeof(CubeType) * 8)];
            std::memset(types, 0xFF, 1 << (sizeof(CubeType) * 8));
            CubeType number_of_types = 0;
            CubeType ttypes[16];

            CubeType const* cubes = chunk.GetCubes();
            for (unsigned int i = 0; i < Common::ChunkSize3; ++i)
            {
                if (types[cubes[i]] == 0xFF)
                {
                    types[cubes[i]] = static_cast<Uint8>(number_of_types);
                    ttypes[number_of_types++] = cubes[i];
                    if (number_of_types > 16)
                        break;
                }
            }
            if (number_of_types == 1)
            {
                p.Write8(1);
                static_assert(sizeof(CubeType) == 2, "faut changer le write16");
                p.Write16(cubes[0]);
            }
            else if (number_of_types <= 16)
            {
                p.Write8(static_cast<Uint8>(number_of_types));
                for (Uint8 i = 0 ; i < number_of_types ; ++i)
                {
                    static_assert(sizeof(CubeType) == 2, "faut changer le write16");
                    p.Write16(ttypes[i]);
                }
                Uint16 curType = cubes[0];
                Uint16 size = 1;
                for (unsigned int i = 1 ; i < Common::ChunkSize3 ; ++i)
                {
                    if (cubes[i] == curType)
                    {
                        if (size == 0xFFF)
                        {
                            size |= (types[curType] & 0xF) << 12;
                            p.Write16(size);
                            curType = cubes[i];
                            size = 1;
                        }
                        else
                            ++size;
                    }
                    else
                    {
                        size &= 0x0FFF;
                        size |= (types[curType] & 0xF) << 12;
                        p.Write16(size);
                        curType = cubes[i];
                        size = 1;
                    }
                }
                size &= 0x0FFF;
                size |= (types[curType] & 0xF) << 12;
                p.Write16(size);
            }
            else
            {
                Uint16 curType = cubes[0];
                Uint8 size = 1;
                for (unsigned int i = 1 ; i < Common::ChunkSize3 ; ++i)
                {
                    if (cubes[i] == curType && size != 255)
                        ++size;
                    else
                    {
                        p.Write8(size);
                        static_assert(sizeof(CubeType) == 2, "faut changer le write16");
                        p.Write16(curType);
                        curType = cubes[i];
                        size = 1;
                    }
                }
                p.Write8(size);
                static_assert(sizeof(CubeType) == 2, "faut changer le write16");
                p.Write16(curType);
            }
        }

    private:
        static void Read(Tools::ByteArray const&, ChunkType&); // Used by ByteArray::Read<T>(T&), but not usable
    };

}

#endif
