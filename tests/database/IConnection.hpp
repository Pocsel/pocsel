#ifndef __TOOLS_DATABASE_ICONNECTION_HPP__
#define __TOOLS_DATABASE_ICONNECTION_HPP__

namespace Tools { namespace Database {
    class IConnection;
    class IQuery;
    class IRow;
}}

namespace Tools { namespace Database {

    class IConnection
    {
    public:
        virtual ~IConnection() {}
        virtual std::unique_ptr<IQuery> CreateQuery(std::string const& request) = 0;
        virtual void BeginTransaction() = 0;
        virtual void EndTransaction() = 0;
    };

    class IQuery
    {
    public:
        virtual ~IQuery() {}
        virtual IQuery& Bind(char val) = 0;
        virtual IQuery& Bind(unsigned char val) = 0;
        virtual IQuery& Bind(short val) = 0;
        virtual IQuery& Bind(unsigned short val) = 0;
        virtual IQuery& Bind(int val) = 0;
        virtual IQuery& Bind(unsigned int val) = 0;
        virtual IQuery& Bind(long long val) = 0;
        virtual IQuery& Bind(unsigned long long val) = 0;
        virtual IQuery& Bind(float val) = 0;
        virtual IQuery& Bind(double val) = 0;
        virtual IQuery& Bind(std::string const& val) = 0;
        virtual IQuery& Bind(void const* tab, std::size_t size) = 0;
        virtual IQuery& ExecuteNonSelect() = 0;
        virtual std::unique_ptr<IRow> Fetch() = 0;
        virtual void Reset() = 0;
    };

    class IRow
    {
    public:
        virtual ~IRow() {}
        virtual char GetChar(int colomn) const = 0;
        virtual unsigned char GetUChar(int colomn) const = 0;
        virtual short GetShort(int colomn) const = 0;
        virtual unsigned short GetUShort(int colomn) const = 0;
        virtual int GetInt(int colomn) const = 0;
        virtual unsigned int GetUInt(int colomn) const = 0;
        virtual long long GetLong(int colomn) const = 0;
        virtual unsigned long long GetULong(int colomn) const = 0;
        virtual float GetFloat(int colomn) const = 0;
        virtual double GetDouble(int colomn) const = 0;
        virtual std::string GetString(int colomn) const = 0;
        virtual std::vector<char> GetArray(int colomn) const = 0;
    };

}}

#endif
