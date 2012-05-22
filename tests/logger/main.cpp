#include "tests/logger/precompiled.hpp"

#include "tools/Timer.hpp"

void TestRegisterCallback(std::string const& prefix, std::string const& line)
{
    std::cout << prefix << ": " << line;
}

bool cmp(std::pair<double, std::string> const& t1, std::pair<double, std::string> const& t2)
{
    return t1.first - t2.first < 0;
}

int main()
{
    Tools::debug.RegisterCallback(std::bind(&TestRegisterCallback, "Callback debug", std::placeholders::_1));
    Tools::log.RegisterCallback(std::bind(&TestRegisterCallback, "Callback log", std::placeholders::_1));
    Tools::error.RegisterCallback(std::bind(&TestRegisterCallback, "Callback error", std::placeholders::_1));

    Tools::debug << "Tools::debug" << std::endl;
    Tools::log << "Tools::log" << std::endl;
    Tools::error << "Tools::error" << std::endl;


    std::multimap<double, std::string> map;
    std::vector<std::pair<double, std::string>> list(100000);

    int total = 0;

    Tools::Timer timer;
    for (double i = 0; i < 100000; ++i)
        map.insert(std::multimap<double, std::string>::value_type(std::sin(i), "coucou"));
    for (int i = 0; i < 50; ++i)
        for (auto it = map.begin(), ite = map.end(); it != ite; ++it)
            total += it->second.c_str()[0];
    auto mapTime = timer.GetPreciseElapsedTime();

    timer.Reset();
    for (double i = 0; i < 100000; ++i)
        list.push_back(std::make_pair<double, std::string>(std::sin(i), "coucou"));
    //list.sort(&cmp);
    std::sort(list.begin(), list.end(), &cmp);
    for (int i = 0; i < 50; ++i)
        for (auto it = list.begin(), ite = list.end(); it != ite; ++it)
            total += it->second.c_str()[0];
    auto listTime = timer.GetPreciseElapsedTime();

    Tools::log << "map: " << mapTime << " list: " << listTime << " diff: " << (long long)(mapTime - listTime) << std::endl;

#ifdef _WIN32
    std::cout << "Press enter\n";
    std::cin.get();
#endif
    return 0;
}
