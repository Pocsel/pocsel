
#include <iostream>
#include <vector>
#include <cstring>

#include "common/Vector2.hpp"
#include "common/Vector3.hpp"
#include "Assert.hpp"

template<class T>
void DoTests()
{
    glm::detail::tvec2<T> a(10, 0);
    glm::detail::tvec2<T> b(20, 0);
    glm::detail::tvec3<T> c(10, 0, 5);
    glm::detail::tvec3<T> d(20, 0, 5);

    Assert(glm::detail::tvec2<T>::CheckDistance(a, b, 10));
    Assert(glm::detail::tvec2<T>::GetDistance(a, b) == 10);
    Assert(glm::detail::tvec2<T>::GetDistanceSquared(a, b) == 10*10);

    Assert(glm::detail::tvec3<T>::CheckDistance(c, d, 10));
    Assert(glm::detail::tvec3<T>::GetDistance(c, d) == 10);
    Assert(glm::detail::tvec3<T>::GetDistanceSquared(c, d) == 10*10);

    Assert(a == a);
    Assert(c == c);

    std::cout << ToString(a) << " / " << ToString(c) << std::endl;
    a = glm::normalize(a);
    a *= 10;
    b = glm::normalize(b);
    b *= 10;
    Assert(a == b);

    c = glm::normalize(c);
    d = Normalize(d);

    a = 2*(a + b - b/5); // Plein de constructions
    c = 2*(c + d - d/5);
    a /= 2; // Pas de copie/construction
    c /= 2;
    a += b;
    c += d;
    a -= b;
    c -= d;
    std::cout << ToString(a) << " / " << ToString(c) << std::endl;
}

int main(int, char**)
{
    DoTests<float>();
    DoTests<double>();
    DoTests<Int8>();
    DoTests<Int16>();
    DoTests<Int32>();
    DoTests<Int64>();
    DoTests<Uint8>();
    DoTests<Uint16>();
    DoTests<Uint32>();
    DoTests<Uint64>();

    return 0;
}
