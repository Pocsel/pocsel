#ifndef __TOOLS_ZLIB_WORKER_HPP__
#define __TOOLS_ZLIB_WORKER_HPP__

#include "tools/database/"

namespace Tools { namespace Zlib {

    class Worker
    {
    private:
        static const unsigned int _chunkSize = 16384;
        unsigned char _in[ChunkSize];
        unsigned char _out[ChunkSize];
        int _compressionLevel;

    public:
        Worker(int compressionLevel);
        void Inflate(void* src, unsigned int srcLength,
                void*& dst, unsigned int *& dstLenght);
        void Deflate(void* src, unsigned int srcLength,
                void*& dst, unsigned int *& dstLenght);

    private:
        std::string _GetError(int ret);
    };

}}

#endif
