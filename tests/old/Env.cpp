
#include <iostream>
#include <sstream>

#include "common/Env.hpp"
#include "Assert.hpp"

void changedBite(std::string const&, Common::Env::Variable& val)
{
    Assert(val.GetType() == Common::Env::Variable::String);
    val = "Non tu change pas";
}

int main()
{
    Common::Env env;

    env.Set("bite", "pif");
    std::string res = env.Get("bite");
    Assert(env.Get("bite") == "pif");
    env.Set("bite", "paf");
    Assert(env.Get("bite") == "paf");
    env.Set("int", 12);
    Assert(env.Get("int") == 12);
    env.Set("f", 14.15432f);
    Assert(env.Get("f") == 14.15432f);

    std::stringstream ss;
    ss << env.Get("bite") << ' ' << env.Get("int");
    Assert(ss.str() == "paf 12");

    // var callback
    env.Set("bite", "Une string quelconque");

    env.Get("bite").PushCallback(changedBite);

    Assert(env.Get("bite") == "Une string quelconque");

    env.Set("bite", "Une string quelconque differente");
    Assert(env.Get("bite") != "Une string quelconque differente");

    env.Get("bite").ClearCallbacks();

    env.Set("bite", "Une string quelconque differente");
    Assert(env.Get("bite") == "Une string quelconque differente");


    return 0;
}

