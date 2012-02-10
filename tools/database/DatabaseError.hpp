#ifndef __TOOLS_DATABASE_DATABASEERROR_HPP__
#define __TOOLS_DATABASE_DATABASEERROR_HPP__

namespace Tools { namespace Database {

    class DatabaseError : public std::exception
    {
    private:
        std::string _msg;

    public:
        DatabaseError(std::string const& msg) : _msg(msg) {}
        virtual const char* what() const throw() { return this->_msg.c_str(); }
        virtual ~DatabaseError() throw() {}
    };

}}

#endif
