
#include <iostream>
#include <stdexcept>
#include <utility>


#include "Exception.hpp"
#include "Interpreter.hpp"
#include "StackRef.hpp"
#include "Setters.hpp"

using namespace Tools::Lua;

Interpreter::Interpreter(std::string const& code)
{
    Exec(code);
}

Interpreter::Interpreter()
{
}

void Interpreter::Load(std::string const& code)
{
    api.loadstring(code.c_str());
}

void Interpreter::Exec(std::string const& code)
{
    api.dostring(code.c_str());
}

void Interpreter::ExecFile(std::string const& path)
{
    api.dofile(path.c_str());
}

Interpreter::~Interpreter()
{
    auto it = this->_bound.begin(),
         end = this->_bound.end();
    for (;it != end; ++it)
        delete *it;
}

Object Interpreter::operator [] (char const* key)
{
    api.getglobal(key);
    StackRefPtr newRef(new StackRef(api));
    return Object(newRef, GlobalSetter(newRef, key));
}

Object Interpreter::operator [] (std::string const& key)
{
    return (*this)[key.c_str()];
}


void Interpreter::DumpStack()
{
    std::cout << "------------------------------- Dump stack\n";
    int top = api.gettop();
    for (int i = 1; i <= top; i++)
    {
        std::cout << i << ": " << api.typestring(i) << ": ";
        if (api.isnumber(i)) std::cout << api.tonumber(i);
        else if (api.isstring(i)) std::cout << api.tostring(i);
        else if (api.isboolean(i)) std::cout << (api.toboolean(i) ? "true" : "false");
        else std::cout << api.topointer(i);
        std::cout << '\n';
    }
    std::cout << "------------------------------- top = " << top << "\n";
}

