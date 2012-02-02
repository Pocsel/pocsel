
#include <cassert>
#include <cstddef>
int main()
{
    char* s = 0;
    assert(s == 0);
    assert(!s);
    assert(s == nullptr);
    std::nullptr_t myNullValue;
    assert(myNullValue == 0);
    assert(!myNullValue);
    assert(myNullValue == s);
}
