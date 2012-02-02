
#include <cassert>
#include <iostream>
#include <thread>

std::mutex lock;
volatile int count = 0;

void f(int idx)
{
    for (int i = 0; i < 1000; ++i)
    {
        std::lock_guard<std::mutex> guard(lock);
        ++count;
    }
}

int main(int ac, char** av)
{
    int i;

    const int NbThreads = (ac > 1 ? atoi(av[1]) : 100);

    std::cout << "NbThreads = " << NbThreads << std::endl;
    std::thread *threads = new std::thread[NbThreads];



    for(i = 0; i < NbThreads; ++i)
        threads[i] = std::thread(&f, i);

    for (i = 0; i < NbThreads ; ++i)
    {
        assert(threads[i].joinable());
        threads[i].join();
    }

    return 0;
}
