#ifndef __TOOLS_DATABASE_IVALUE_HPP__
#define __TOOLS_DATABASE_IVALUE_HPP__

namespace Tools { namespace Database {

    struct Blob
    {
        size_t const size;
        void const* data;

        Blob(void const* d, size_t s) :
            size(s), data(d)
        {
        }
        Blob(Blob const& b) :
            size(b.size), data(b.data)
        {
        }
    private:
        Blob& operator =(Blob const& b);
    };

    class IValue
    {
    public:
        virtual int GetInt() = 0;
        virtual Int32 GetInt32() = 0;
        virtual Uint32 GetUint32() = 0;
        virtual Int64 GetInt64() = 0;
        virtual Uint64 GetUint64() = 0;
        virtual float GetFloat() = 0;
        virtual double GetDouble() = 0;
        virtual std::string GetString() = 0;
        virtual bool GetBool() = 0;
        virtual Blob GetBlob() = 0;
        virtual ~IValue()
        {
        }
    };

}}

#endif
