#ifndef __COMMON_RESOURCE_HPP__
#define __COMMON_RESOURCE_HPP__

namespace Common {

    class Resource : private boost::noncopyable
    {
    public:
        Uint32 const id;
        //Uint32 const pluginId;
        std::string const type;
        std::string const name;
        void const* const data;
        Uint32 const size;
    private:
        bool _isValid;

    public:
        // TODO: constructeur � virer (avec pluginId) lorsque le packager mettera les bons noms de resources ("pluginName:resourceName")
        //Resource(Uint32 id,
        //        Uint32 pluginId,
        //        std::string const& type,
        //        std::string const& name,
        //        void const* data,
        //        Uint32 size) :
        //    id(id),
        //    pluginId(pluginId),
        //    type(type),
        //    name(name),
        //    data(_CopyData(data, size)),
        //    size(size)
        //{}

        Resource(Uint32 id,
                std::string const& type,
                std::string const& name,
                void const* data,
                Uint32 size) :
            id(id),
            //pluginId(0),
            type(type),
            name(name),
            data(_CopyData(data, size)),
            size(size),
            _isValid(true)
        {
        }

        Resource(Resource&& other) :
            id(other.id),
            type(other.type),
            name(other.name),
            data(other.data),
            size(other.size)
        {
            other._isValid = false;
        }

        ~Resource()
        {
            if (this->_isValid)
                delete [] ((char*) this->data);
        }

        bool IsValid() const { return this->_isValid; }

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
