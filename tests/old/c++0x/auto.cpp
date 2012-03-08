
#include <string>

template<typename T>
struct S
{
    void m(T const& i) const {}
};

int main()
{
    std::string s;
    auto it = s.begin(), end = s.end();
    auto bite = s;
    auto bite2 = s + "salut";
    auto bite3 = &S<int>::m;
    auto bite5 = &S<void*>::m;
    auto bite6 = "Ho une string";

    return 0;
}
