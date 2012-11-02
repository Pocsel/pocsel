#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <luasel/Interpreter.hpp>
#include <luasel/Iterator.hpp>

namespace Luasel {

    Serializer::Serializer(Interpreter& interpreter) :
        _interpreter(interpreter)
    {
    }

    std::string Serializer::Serialize(Ref const& ref, bool nilOnError /* = false */) const throw(std::runtime_error)
    {
        std::list<Ref> tables;
        return this->_Serialize(ref, tables, 1, nilOnError);
    }

    Ref Serializer::Deserialize(std::string const& string) const throw(std::runtime_error)
    {
        if (string.empty())
            return Ref(this->_interpreter.GetState());
        return this->_interpreter.LoadString("return " + string)();
    }

    Ref Serializer::MakeSerializableCopy(Ref const& ref, bool nilOnError /* = false */) const throw(std::runtime_error)
    {
        std::list<Ref> tables;
        return this->_Copy(ref, tables, nilOnError);
    }

    Ref Serializer::_Copy(Ref const& ref, std::list<Ref>& tables, bool nilOnError) const throw(std::runtime_error)
    {
        if (ref.IsTable())
        {
            auto findIt = std::find(tables.begin(), tables.end(), ref);
            if (findIt != tables.end())
            {
                if (nilOnError)
                    return this->_interpreter.MakeNil();
                throw std::runtime_error("Luasel::Serializer: Cyclic or shared table(s) found in table to copy and serialize");
            }
            tables.push_front(ref);
            auto metaTable = ref.GetMetaTable();
            if (metaTable.IsTable())
                return this->_CopyWithMetaTable(ref, metaTable, nilOnError);
            auto copiedTable = this->_interpreter.MakeTable();
            auto it = ref.Begin();
            auto itEnd = ref.End();
            for (; it != itEnd; ++it)
                try
                {
                    copiedTable.Set(this->_Copy(it.GetKey(), tables, nilOnError), this->_Copy(it.GetValue(), tables, nilOnError));
                }
                catch (std::exception&)
                {
                    // can only throw if nilOnError is true and that the key is either a non-copyable userdata, a cyclic table or a non-copyable "object table" (meaning an index equal to nil)
                    // so we do nothing because we want nil on error
                }
            return copiedTable;
        }
        else
            return this->_CopySimpleValue(ref, nilOnError);
    }

    Ref Serializer::_CopySimpleValue(Ref const& ref, bool nilOnError) const throw(std::runtime_error)
    {
        if (ref.IsNumber() || ref.IsBoolean() || ref.IsString() || ref.IsNil())
            return ref;
        else if (ref.IsUserData())
        {
            auto metaTable = ref.GetMetaTable();
            if (metaTable.IsTable())
                return this->_CopyWithMetaTable(ref, metaTable, nilOnError);
            // userdata with no metatable (weird)
            if (nilOnError)
                return this->_interpreter.MakeNil();
            throw std::runtime_error("Luasel::Serializer: User data with no metatable");
        }
        // other type -> error
        if (nilOnError)
            return this->_interpreter.MakeNil();
        throw std::runtime_error("Luasel::Serializer: Value of type " + ref.GetTypeName() + " is not copyable or serializable");
    }

    Ref Serializer::_CopyWithMetaTable(Ref const& ref, Ref const& table, bool nilOnError) const throw(std::runtime_error)
    {
        auto cloneFunc = table["__clone"];
        if (!cloneFunc.IsFunction())
        {
            if (nilOnError)
                return this->_interpreter.MakeNil();
            throw std::runtime_error("Luasel::Serializer: Cannot copy this " + ref.GetTypeName() + " (the value at \"__clone\" is of type \"" + cloneFunc.GetTypeName() + "\")");
        }
        try
        {
            return cloneFunc(ref);
        }
        catch (std::exception& e)
        {
            if (nilOnError)
                return this->_interpreter.MakeNil();
            throw std::runtime_error("Luasel::Serializer: Cannot copy this " + ref.GetTypeName() + ": " + e.what());
        }
    }

    std::string Serializer::_SerializeSimpleValue(Ref const& ref, bool nilOnError) const throw(std::runtime_error)
    {
        if (ref.IsNumber())
            return boost::lexical_cast<std::string>(ref.ToNumber());
        else if (ref.IsBoolean())
            return ref.ToBoolean() ? "true" : "false";
        else if (ref.IsString())
            return this->_SerializeString(ref.ToString());
        else if (ref.IsNil())
            return "nil";
        else if (ref.IsUserData())
        {
            auto metaTable = ref.GetMetaTable();
            if (metaTable.IsTable())
                return this->_SerializeWithMetaTable(ref, metaTable, nilOnError);
            // userdata with no metatable (weird)
            if (nilOnError)
                return "nil";
            throw std::runtime_error("Luasel::Serializer: User data with no metatable");
        }
        // other type -> error
        if (nilOnError)
            return "nil";
        throw std::runtime_error("Luasel::Serializer: Type " + ref.GetTypeName() + " is not serializable");
    }

    std::string Serializer::_SerializeWithMetaTable(Ref const& ref, Ref const& table, bool nilOnError) const throw(std::runtime_error)
    {
        auto serializeFunc = table["__serialize"];
        if (!serializeFunc.IsFunction())
        {
            if (nilOnError)
                return "nil";
            throw std::runtime_error("Luasel::Seralizer: Cannot serialize this " + ref.GetTypeName() + " (the value at \"__serialize\" is of type \"" + serializeFunc.GetTypeName() + "\")");
        }
        try
        {
            auto res = serializeFunc(ref);
            if (!res.IsString())
                throw std::runtime_error("Luasel::Serializer: Return value of serialization function for type " + ref.GetTypeName() + " is of type " + res.GetTypeName());
            return "(function() " + res.ToString() + " end)()";
        }
        catch (std::exception& e)
        {
            if (nilOnError)
                return "nil";
            throw std::runtime_error("Luasel::Serializer: Cannot serialize this " + ref.GetTypeName() + ": " + e.what());
        }
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
                ret += boost::lexical_cast<std::string>(static_cast<unsigned int>(c));
            }
            else
                ret += c;
        }
        ret += '"';
        return ret;
    }

    std::string Serializer::_Serialize(Ref const& ref, std::list<Ref>& tables, unsigned int level, bool nilOnError) const throw(std::runtime_error)
    {
        if (ref.IsTable())
        {
            auto findIt = std::find(tables.begin(), tables.end(), ref);
            if (findIt != tables.end())
            {
                if (nilOnError)
                    return "nil";
                throw std::runtime_error("Luasel::Serializer: Cyclic or shared table(s) found in table to serialize");
            }
            tables.push_front(ref);
            auto metaTable = ref.GetMetaTable();
            if (metaTable.IsTable())
                return this->_SerializeWithMetaTable(ref, metaTable, nilOnError);
            std::string ret = "{\n";
            auto it = ref.Begin();
            auto itEnd = ref.End();
            for (; it != itEnd; ++it)
            {
                std::string key = this->_Serialize(it.GetKey(), tables, 0, nilOnError);
                if (key != "nil") // peut être nil uniquement si nilOnError == true et que la key est un userdata non serializable ou une table cyclique ou une "table objet" non serializable
                {
                    for (unsigned int i = 0; i < level; ++i)
                        ret += "\t";
                    ret += "[" + key + "] = " + this->_Serialize(it.GetValue(), tables, level + 1, nilOnError) + ",\n";
                }
            }
            for (unsigned int i = 0; i < level - 1; ++i)
                ret += "\t";
            ret += "}";
            return ret;
        }
        else
            return this->_SerializeSimpleValue(ref, nilOnError);
    }

}
