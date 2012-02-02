
#include <unordered_map>

int main()
{
    std::unordered_map<int, char const*>  m;
    m[12] = "bite";
    (m.begin())->second = "pif";
}

