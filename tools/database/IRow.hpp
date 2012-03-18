#ifndef __TOOLS_DATABASE_IROW_HPP__
#define __TOOLS_DATABASE_IROW_HPP__

namespace Tools { namespace Database {

    class IRow
    {
    public:
        virtual ~IRow() {}

        virtual Uint32 GetColumnCount() const = 0;
        virtual Int8 GetInt8(Uint32 column) const = 0;
        virtual Uint8 GetUint8(Uint32 column) const = 0;
        virtual Int16 GetInt16(Uint32 column) const = 0;
        virtual Uint16 GetUint16(Uint32 column) const = 0;
        virtual Int32 GetInt32(Uint32 column) const = 0;
        virtual Uint32 GetUint32(Uint32 column) const = 0;
        virtual Int64 GetInt64(Uint32 column) const = 0;
        virtual Uint64 GetUint64(Uint32 column) const = 0;
        virtual float GetFloat(Uint32 column) const = 0;
        virtual double GetDouble(Uint32 column) const = 0;
        virtual std::string GetString(Uint32 column) const = 0;
        virtual std::vector<char> GetArray(Uint32 column) const = 0;
    };

}}

#endif
