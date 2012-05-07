#ifndef __COMMON_CONSTANTS_HPP__
#define __COMMON_CONSTANTS_HPP__

namespace Common {
    // static is needed when use in tab
    static Uint32 const ChunkSize = 32;
    static Uint32 const ChunkSize2 = ChunkSize * ChunkSize;
    static Uint32 const ChunkSize3 = ChunkSize2 * ChunkSize;

    static Uint32 PluginFormatVersion = 1;

    extern char const* const PluginFileExt;
    extern char const* const WorldFileExt;
    extern char const* const CacheFileExt;
}

#endif
