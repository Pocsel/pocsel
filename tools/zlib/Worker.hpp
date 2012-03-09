#ifndef __TOOLS_ZLIB_WORKER_HPP__
#define __TOOLS_ZLIB_WORKER_HPP__

namespace Tools { namespace Zlib {

    class Worker
    {
    private:
        enum
        {
            _chunkSize = 16384
        };

        unsigned char _out[_chunkSize];
        int _compressionLevel;

    public:
        Worker(int compressionLevel);
        void Inflate(void const* src, unsigned int srcLength,
                void*& dst, unsigned int& dstLenght);
        void Deflate(void const* src, unsigned int srcLength,
                void*& dst, unsigned int& dstLenght);

    private:
        std::string _GetError(int ret);
    };

}}

#endif
