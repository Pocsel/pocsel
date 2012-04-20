
#include "tests/logger/precompiled.hpp"

void TestRegisterCallback(std::string const& prefix, std::string const& line)
{
    std::cout << prefix << ": " << line;
}

int main()
{
    Tools::debug.RegisterCallback(std::bind(&TestRegisterCallback, "Callback debug", std::placeholders::_1));
    Tools::log.RegisterCallback(std::bind(&TestRegisterCallback, "Callback log", std::placeholders::_1));
    Tools::error.RegisterCallback(std::bind(&TestRegisterCallback, "Callback error", std::placeholders::_1));

    Tools::debug << "Tools::debug" << std::endl;
    Tools::log << "Tools::log" << std::endl;
    Tools::error << "Tools::error" << std::endl;

#ifdef _WIN32
    std::cout << "Press enter\n";
    std::cin.get();
#endif
    return 0;
}
