#ifndef __TOOLS_DATABASE_IROW_HPP__
#define __TOOLS_DATABASE_IROW_HPP__

namespace Tools { namespace Database {

    class IRow
    {
    public:
        virtual ~IRow() {}
        virtual char GetChar(int colomn) const = 0;
        virtual unsigned char GetUchar(int colomn) const = 0;
        virtual short GetInt16(int colomn) const = 0;
        virtual unsigned short GetUint16(int colomn) const = 0;
        virtual int GetInt(int colomn) const = 0;
        virtual unsigned int GetUint(int colomn) const = 0;
        virtual long long GetInt64(int colomn) const = 0;
        virtual unsigned long long GetUint64(int colomn) const = 0;
        virtual float GetFloat(int colomn) const = 0;
        virtual double GetDouble(int colomn) const = 0;
        virtual std::string GetString(int colomn) const = 0;
        virtual std::vector<char> GetArray(int colomn) const = 0;
    };

}}

#endif
