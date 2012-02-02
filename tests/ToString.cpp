
#include <iostream>

#include "tools/ToString.hpp"
#include "Assert.hpp"

//// Stringifier un type :

// le type
struct MyType {int i;};

namespace Tools {
    // A mettre dans le header du type
    template<> struct Stringify<MyType>
    {
        static inline std::string MakeString(MyType const& var)
        {
            return "<MyType: i = " + ToString(var.i) + ">";
            // quizz: Combien de constructeurs de string sont appelés ?
        }
    };
}

// BITE
#define ASSERT_EQ(o, s) Assert(Tools::ToString(o) == std::string(s))
#define ASSERT_NE(o, s) Assert(Tools::ToString(o) != std::string(s))

int main()
{
    ASSERT_EQ(8, "8");
    ASSERT_EQ("8", "8");
    ASSERT_EQ(true, "true");
    ASSERT_EQ(false, "false");

    char const* str = "bite";
    ASSERT_EQ(str, "bite");
    ASSERT_EQ(str, "bite");
    ASSERT_NE((void*) str, "bite"); // nb maximal de bite / fichier atteint

    MyType var;
    var.i = 42;
    ASSERT_EQ(var, "<MyType: i = 42>");
    std::cout << "tests done.\n";
    return 0;
}
