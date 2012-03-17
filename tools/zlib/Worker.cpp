#include <zlib.h>

#include "tools/zlib/Worker.hpp"

namespace Tools { namespace Zlib {

    namespace {

        unsigned int _Max(unsigned int a, unsigned int b)
        {
            if (a >= b)
                return a;
            return b;
        }
        unsigned int _Min(unsigned int a, unsigned int b)
        {
            if (a < b)
                return a;
            return b;
        }
    }

    Worker::Worker(int compressionLevel)
    {
        if (compressionLevel < -1)
            this->_compressionLevel = -1;
        else if (compressionLevel > 9)
            this->_compressionLevel = 9;
        else
            this->_compressionLevel = compressionLevel;
    }

    void Worker::Deflate(void const* _src, unsigned int srcLength,
            void*& _dst, unsigned int& dstLenght)
    {
        int ret, flush;
        unsigned int have;
        z_stream strm;
        unsigned char* src = (unsigned char*)_src;
        std::vector<unsigned char> dst;



        /* allocate deflate state */
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        ret = deflateInit2(&strm,
                           this->_compressionLevel,
                           Z_DEFLATED,
                           -15,
                           9,
                           Z_DEFAULT_STRATEGY);
        if (ret != Z_OK)
        {
            throw std::runtime_error(std::string("FAIL: ") + this->_GetError(ret) + "\n");;
        }

        /* compress until end of file */
        do
        {
            strm.avail_in = _Min(_chunkSize, srcLength);
            //strm.avail_in = fread(in, 1, CHUNK, source);
            //if (ferror(source)) {
            //    (void)deflateEnd(&strm);
            //    return Z_ERRNO;
            //}


            strm.next_in = src;
            src += strm.avail_in;
            srcLength -= strm.avail_in;

            //flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
            flush = srcLength == 0 ? Z_FINISH : Z_NO_FLUSH;


            /* run deflate() on input until output buffer not full, finish
               compression if all of source has been read in */
            do
            {
                strm.avail_out = _chunkSize;
                strm.next_out = _out;
                ret = deflate(&strm, flush);    /* no bad return value */
                assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
                have = _chunkSize - strm.avail_out;

                unsigned int curIndex = dst.size();
                dst.resize(dst.size() + have);
                std::memcpy(dst.data() + curIndex, _out, have);

                //if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                //    (void)deflateEnd(&strm);
                //    return Z_ERRNO;
                //}
            }
            while (strm.avail_out == 0);
            assert(strm.avail_in == 0);     /* all input will be used */

            /* done when last data in file processed */
        }
        while (flush != Z_FINISH);
        assert(ret == Z_STREAM_END);        /* stream will be complete */

        /* clean up and return */
        (void)deflateEnd(&strm);

        _dst = new unsigned char[dst.size()];

        std::memcpy(_dst, dst.data(), dst.size());

        dstLenght = dst.size();
    }

    void Worker::Inflate(void const* _src, unsigned int srcLength,
            void*& _dst, unsigned int& dstLenght)
    {
        int ret;
        unsigned int have;
        z_stream strm;
        unsigned char* src = (unsigned char*)_src;
        std::vector<unsigned char> dst;

        /* allocate inflate state */
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;
        ret = inflateInit2(&strm, -15);
        if (ret != Z_OK)
        {
            throw std::runtime_error(std::string("FAIL: ") + this->_GetError(ret) + "\n");;
        }

        /* decompress until deflate stream ends or end of file */
        do
        {
            strm.avail_in = _Min(_chunkSize, srcLength);
            //strm.avail_in = fread(in, 1, CHUNK, source);
            //if (ferror(source))
            //{
            //    (void)inflateEnd(&strm);
            //    return Z_ERRNO;
            //}
            if (strm.avail_in == 0)
                break;

            strm.next_in = src;

            srcLength -= strm.avail_in;
            src += strm.avail_in;

            /* run inflate() on input until output buffer not full */
            do
            {
                strm.avail_out = _chunkSize;
                strm.next_out = _out;
                ret = inflate(&strm, Z_NO_FLUSH);
                assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
                switch (ret)
                {
                    case Z_NEED_DICT:
                        ret = Z_DATA_ERROR;     /* and fall through */
                    case Z_DATA_ERROR:
                    case Z_MEM_ERROR:
                        (void)inflateEnd(&strm);
                        throw std::runtime_error(std::string("FAIL: ") + this->_GetError(ret) + "\n");;
                }
                have = _chunkSize - strm.avail_out;

                unsigned int curIndex = dst.size();
                dst.resize(dst.size() + have);
                std::memcpy(dst.data() + curIndex, _out, have);
                //if (fwrite(out, 1, have, dest) != have || ferror(dest))
                //{
                //    (void)inflateEnd(&strm);
                //    return Z_ERRNO;
                //}
            } while (strm.avail_out == 0);

            /* done when inflate() says it's done */
        } while (ret != Z_STREAM_END);

        /* clean up and return */
        (void)inflateEnd(&strm);


        _dst = new unsigned char[dst.size()];

        std::memcpy(_dst, dst.data(), dst.size());


        dstLenght = dst.size();
    }

    std::string Worker::_GetError(int ret)
    {
        switch (ret)
        {
            case Z_ERRNO:
                return "unknown error";
            case Z_STREAM_ERROR:
                return "invalid compression level";
            case Z_DATA_ERROR:
                return "invalid or incomplete deflate data";
            case Z_MEM_ERROR:
                return "out of memory";
            case Z_VERSION_ERROR:
                return "zlib version mismatch!";
            default:
                return "fail";
        }
    }

}}
