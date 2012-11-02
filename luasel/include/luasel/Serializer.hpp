#ifndef __LUASEL_SERIALIZER_HPP__
#define __LUASEL_SERIALIZER_HPP__

namespace Luasel {

    class Interpreter;

    class Serializer :
        private boost::noncopyable
    {
        private:
            Interpreter& _interpreter;

        public:
            Serializer(Interpreter& interpreter);
            std::string Serialize(Ref const& ref, bool nilOnError = false) const throw(std::runtime_error);
            Ref Deserialize(std::string const& string) const throw(std::runtime_error);
            Ref MakeSerializableCopy(Ref const& ref, bool nilOnError = false) const throw(std::runtime_error);
        private:
            Ref _Copy(Ref const& ref, std::list<Ref>& tables, bool nilOnError) const throw(std::runtime_error);
            Ref _CopySimpleValue(Ref const& ref, bool nilOnError) const throw(std::runtime_error);
            Ref _CopyWithMetaTable(Ref const& ref, Ref const& table, bool nilOnError) const throw(std::runtime_error);
            std::string _SerializeSimpleValue(Ref const& ref, bool nilOnError) const throw(std::runtime_error);
            std::string _SerializeString(std::string const& string) const;
            std::string _SerializeWithMetaTable(Ref const& ref, Ref const& table, bool nilOnError) const throw(std::runtime_error);
            std::string _Serialize(Ref const& ref, std::list<Ref>& tables, unsigned int level, bool nilOnError) const throw(std::runtime_error);
    };

}

#endif
