#ifndef __COMMON_RESOURCE_HPP__
#define __COMMON_RESOURCE_HPP__

#include <cstring>
#include <memory>

#include <boost/noncopyable.hpp>

namespace Common {

    class Resource : private boost::noncopyable
    {
    public:
        Uint32 const id;
        Uint32 const pluginId;
        std::string const type;
        std::string const filename;
        void const* const data;
        Uint32 const size;


    public:
        Resource(Uint32 id,
                Uint32 pluginId,
                std::string const& type,
                std::string const& filename,
                void const* data,
                size_t size) :
            id(id),
            pluginId(pluginId),
            type(type),
            filename(filename),
            data(_CopyData(data, size)),
            size(size)
        {}

        ~Resource()
        {
            delete [] ((char*) this->data);
        }

        void WriteDataWithABadConstCast(void const* data, Uint32 size, Uint32 offset)
        {
            if (data && size)
                std::memcpy(static_cast<char*>(const_cast<void*>(this->data)) + offset, data, size);
        }

    private:
        void* _CopyData(void const* data, Uint32 size)
        {
            void* newptr = new char[size];
            if (data)
                std::memcpy(newptr, data, size);
            return newptr;
        }
    };

}

#endif
