#ifndef __TOOLS_DATABASE_IROW_HPP__
#define __TOOLS_DATABASE_IROW_HPP__

namespace Tools { namespace Database {

    class IRow
    {
    public:
        virtual ~IRow() {}
        virtual Int8 GetInt8(int column) const = 0;
        virtual Uint8 GetUint8(int column) const = 0;
        virtual Int16 GetInt16(int column) const = 0;
        virtual Uint16 GetUint16(int column) const = 0;
        virtual Int32 GetInt32(int column) const = 0;
        virtual Uint32 GetUint32(int column) const = 0;
        virtual Int64 GetInt64(int column) const = 0;
        virtual Uint64 GetUint64(int column) const = 0;
        virtual float GetFloat(int column) const = 0;
        virtual double GetDouble(int column) const = 0;
        virtual std::string GetString(int column) const = 0;
        virtual std::vector<char> GetArray(int column) const = 0;
    };

}}

#endif
