#include "tools/lua/Interpreter.hpp"
#include "tools/lua/Iterator.hpp"

namespace Tools { namespace Lua {

    Serializer::Serializer(Interpreter& interpreter) :
        _interpreter(interpreter)
    {
    }

    std::string Serializer::Serialize(Ref const& ref) const throw(std::runtime_error)
    {
        std::list<Ref> tables;
        return "return " + this->_Serialize(ref, tables, 1);
    }

    Ref Serializer::Deserialize(std::string const& string) const throw(std::runtime_error)
    {
        if (string.empty())
            return Ref(this->_interpreter.GetState());
        return this->_interpreter.LoadString(string)();
    }

    Ref Serializer::MakeSerializableCopy(Ref const& ref) const throw(std::runtime_error)
    {
        std::list<Ref> tables;
        return this->_Copy(ref, tables);
    }

    Ref Serializer::_Copy(Ref const& ref, std::list<Ref>& tables) const throw(std::runtime_error)
    {
        if (ref.IsTable())
        {
            auto findIt = std::find(tables.begin(), tables.end(), ref);
            if (findIt != tables.end())
                throw std::runtime_error("Lua::Serializer: Cyclic or shared table(s) found in table to copy and serialize");
            tables.push_front(ref);
            auto copiedTable = this->_interpreter.MakeTable();
            auto it = ref.Begin();
            auto itEnd = ref.End();
            for (; it != itEnd; ++it)
                copiedTable.Set(this->_CopySimpleValue(it.GetKey()), this->_CopySimpleValue(it.GetValue()));
            return copiedTable;
        }
        else
            return this->_CopySimpleValue(ref);
    }

    Ref Serializer::_CopySimpleValue(Ref const& ref) const throw(std::runtime_error)
    {
        if (ref.IsNumber() || ref.IsBoolean() || ref.IsString() || ref.IsNil())
            return ref;
        // TODO user data
        throw std::runtime_error("Lua::Serializer: Value of type " + ref.GetTypeName() + " is not copyable or serializable");
    }

    std::string Serializer::_SerializeSimpleValue(Ref const& ref) const throw(std::runtime_error)
    {
        if (ref.IsNumber())
            return Tools::ToString(ref.ToNumber()); // TODO exponent !!
        else if (ref.IsBoolean())
            return ref.ToBoolean() ? "true" : "false";
        else if (ref.IsString())
            return this->_SerializeString(ref.ToString());
        else if (ref.IsNil())
            return "nil";
        else if (ref.IsUserData())
        {
            auto serialize = ref.GetMetaTable()["__serialize"];
            if (!serialize.Exists())
                throw std::runtime_error("Lua::Seralizer: Cannot serialize this user data");
            return "(function() " + serialize(ref).CheckString() + " end)()";
        }
        throw std::runtime_error("Lua::Serializer: Type " + ref.GetTypeName() + " is not serializable");
    }

    std::string Serializer::_SerializeString(std::string const& string) const
    {
        std::string ret;
        ret += '"';
        auto it = string.begin();
        auto itEnd = string.end();
        for (; it != itEnd; ++it)
        {
            unsigned char c = *it;
            if (c < ' ' || c > '~' || c == '"' || c == '\\')
            {
                ret += '\\';
                if (c < 100)
                    ret += '0';
                if (c < 10)
                    ret += '0';
                ret += Tools::ToString(static_cast<unsigned int>(c));
            }
            else
                ret += c;
        }
        ret += '"';
        return ret;
    }

    std::string Serializer::_Serialize(Ref const& ref, std::list<Ref>& tables, unsigned int level) const throw(std::runtime_error)
    {
        if (ref.IsTable())
        {
            auto findIt = std::find(tables.begin(), tables.end(), ref);
            if (findIt != tables.end())
                throw std::runtime_error("Lua::Serializer: Cyclic or shared table(s) found in table to serialize");
            tables.push_front(ref);
            std::string ret = "{\n";
            auto it = ref.Begin();
            auto itEnd = ref.End();
            for (; it != itEnd; ++it)
            {
                for (unsigned int i = 0; i < level; ++i)
                    ret += "\t";
                ret += "[" + this->_SerializeSimpleValue(it.GetKey()) + "] = " + this->_Serialize(it.GetValue(), tables, level + 1) + ",\n";
            }
            for (unsigned int i = 0; i < level - 1; ++i)
                ret += "\t";
            ret += "}";
            return ret;
        }
        else
            return this->_SerializeSimpleValue(ref);
    }

}}
