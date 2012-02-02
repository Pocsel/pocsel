#include <iostream>
#include <stdexcept>

#include "tools/ToString.hpp"
#include "Env.hpp"

namespace Common {

Env::Env(OnNewCallbackType onNew) :
    _onNew(onNew)
{
}

Env::~Env()
{
    auto it = this->_map.begin(), end = this->_map.end();
    for (; it != end; ++it)
        delete it->second;
    this->_map.clear();
}

void Env::Variable::_FireCallbacks()
{
    if (this->_doNotFire || this->_onChangeCallbacks.size() == 0)
        return;
    this->_doNotFire = true; // avoid recursion
    auto it = this->_onChangeCallbacks.begin(), end = this->_onChangeCallbacks.end();
    for (; it != end; ++it)
    {
        try
        {
            (*it)(this->_name, *this);
        }
        catch (std::exception& err)
        {
            std::cerr << "While firing onChange callback for '" << this->_name << "': "
                      << err.what() << std::endl;
        }
    }
    this->_doNotFire = false;
}

std::ostream& operator <<(std::ostream& out, Env::Variable const& var)
{
    var._Write(out);
    return out;
}

namespace
{
    typedef Env::Variable Var;
    template<Var::Type const From, Var::Type const To>
    std::runtime_error CastError()
    {
        return std::runtime_error("Cannot cast " + Tools::ToString(From) + " to " + Tools::ToString(To));
    }

    struct StringVar : public Var
    {
    public:
        std::string var;
        StringVar(std::string const& name, std::string const& var) : Var(name), var(var) {}
        virtual Var::Type GetType() const { return Var::String; };
        virtual operator std::string const&() { return this->var; }
        virtual operator Int32() { throw CastError<Var::String, Var::Int>(); }
        virtual operator float() { throw CastError<Var::String, Var::Float>(); }
        virtual Variable& operator =(std::string const& str)
        {
            this->var = str;
            this->_FireCallbacks();
            return *this;
        }
        virtual Variable& operator =(Int32) { throw CastError<Var::Int, Var::String>(); }
        virtual Variable& operator =(float) { throw CastError<Var::Float, Var::String>(); }

        virtual bool operator ==(std::string const& str) const{ return this->var == str; }
        virtual bool operator ==(Int32) const { throw CastError<Var::Int, Var::String>(); }
        virtual bool operator ==(float) const { throw CastError<Var::Float, Var::String>(); }
    protected:
        virtual void _Write(std::ostream& out) const { out << this->var; }
    };

    struct IntVar : public Var
    {
        Int32 var;
        IntVar(std::string const& name, Int32 var) : Var(name), var(var) {}
        virtual Var::Type GetType() const { return Var::Int; };
        virtual operator std::string const&() { throw CastError<Var::Int, Var::String>(); }
        virtual operator Int32()  { return this->var; }
        virtual operator float() { throw CastError<Var::Int, Var::Float>(); }
        virtual Variable& operator =(std::string const&) { throw CastError<Var::String, Var::Int>(); }
        virtual Variable& operator =(Int32 i)
        {
            this->var = i;
            this->_FireCallbacks();
            return *this;
        }
        virtual Variable& operator =(float) { throw CastError<Var::Float, Var::Int>(); }

        virtual bool operator ==(std::string const&) const { throw CastError<Var::String, Var::Int>(); }
        virtual bool operator ==(Int32 i) const { return this->var == i; }
        virtual bool operator ==(float) const { throw CastError<Var::Float, Var::Int>(); }
    protected:
        virtual void _Write(std::ostream& out) const { out << this->var; }
    };

    struct FloatVar : public Var
    {
        float var;
        FloatVar(std::string const& name, float var) : Var(name), var(var) {}
        virtual Var::Type GetType() const { return Var::Float; };
        virtual operator std::string const&() { throw CastError<Var::Float, Var::String>(); }
        virtual operator Int32() { throw CastError<Var::Float, Var::Int>(); }
        virtual operator float() { return this->var; }
        virtual Variable& operator =(std::string const&) { throw CastError<Var::String, Var::Float>(); }
        virtual Variable& operator =(Int32) { throw CastError<Var::Int, Var::Float>(); }
        virtual Variable& operator =(float f)
        {
            this->var = f;
            this->_FireCallbacks();
            return *this;
        }

        virtual bool operator ==(std::string const&) const { throw CastError<Var::String, Var::Float>(); }
        virtual bool operator ==(Int32) const { throw CastError<Var::Int, Var::Float>(); }
        virtual bool operator ==(float f) const { return this->var == f; }
    protected:
        virtual void _Write(std::ostream& out) const { out << this->var; }
    };

    template<typename T> struct TypeToVarType;
    template<> struct TypeToVarType<std::string>    { typedef StringVar type; };
    template<> struct TypeToVarType<Int32>          { typedef IntVar type; };
    template<> struct TypeToVarType<float>          { typedef FloatVar type; };

    template<typename T> struct TypeToVarTypeId;
    template<> struct TypeToVarTypeId<std::string>  { static Var::Type const typeId = Var::String; };
    template<> struct TypeToVarTypeId<Int32>        { static Var::Type const typeId = Var::Int; };
    template<> struct TypeToVarTypeId<float>        { static Var::Type const typeId = Var::Float; };
}

Env::Variable& Env::Get(std::string const& key)
{
    auto it = this->_map.find(key);
    if (it == this->_map.end())
        throw std::runtime_error("Cannot find key '" + key + "'");
    return *(it->second);
}

template<typename T>
void Env::Set(std::string const& key, T const& val)
{
    auto it = this->_map.find(key);
    typedef typename TypeToVarType<T>::type VarType;
    if (it == this->_map.end())
    {
        Env::Variable* var = new VarType(key, val);
        this->_map[key] = var;
        if (this->_onNew != 0)
            this->_onNew(key, *var);
    }
    else
    {
        *(it->second) = val;
    }
}

void Env::Set(std::string const& key, char const* val)
{
    return this->Set<std::string>(key, val);
}

bool Env::HasKey(std::string const& key)
{
    return this->_map.find(key) != this->_map.end();
}

//template void Env::Set<std::string>(std::string const& key, std::string const& val);
template void Env::Set<Int32>(std::string const& key, Int32 const& val);
template void Env::Set<float>(std::string const& key, float const& val);

}
