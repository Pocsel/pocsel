

struct P
{
    int f(int) = delete;
};

// impossible
//int P::f(int) {}

struct D : P
{
    // On peut faire ça
    int f(int) {}
};

int main()
{
    P p;
    D d;
}
