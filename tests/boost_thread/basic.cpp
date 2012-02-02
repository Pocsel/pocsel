#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/ref.hpp>

#include "Assert.hpp"

class BasicShit
{
private:
    boost::condition_variable cdt;
    boost::mutex mutmut;
    boost::unique_lock<boost::mutex> lck;
public:
    BasicShit() : lck(mutmut)
    {
        std::cout << "ctor\n";
    }
    BasicShit(BasicShit const&) : lck(mutmut)
    {
        std::cout << "cpy ctor\n";
    }
    void operator()()
    {
        while (1)
        {
//            boost::this_thread::yield(1); pour laisser tourner les autres threads si il faut

            std::cout << "je joue\n";
            this->cdt.wait(lck);
        }
    }
    void WakeUp()
    {
        this->cdt.notify_one();
    }
};

int main()
{
    std::cout << "Comportement attendu : on tape du texte, ça affiche \"caca_prout_je_fonctionne\".\n";
    std::cout << "PS : le constructeur est appelé une seule fois grâce à boost::ref\n";

    BasicShit toto;

    boost::thread t1(boost::ref(toto));

    while (1)
    {
        std::string str;
        std::cin >> str;
        std::cout << str << "\n";
        toto.WakeUp();
    }

    return 0;
}

