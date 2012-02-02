

struct Base
{
    int f(int) {}
};

struct Derived explicit : Base
{
    int f(int) override {}
};

int main()
{
    Derived d;
}
