
int main()
{
    auto my_lambda_func = [&](int x) { };
    auto f = [](int x, int y) { return x + y; };
    f(2, 3);
}
