
#include <iostream>
#include "tools/Timer.hpp"
#include "tools/MessageQueue.hpp"
#include "tools/MessageQueueManager.hpp"


struct Q1 : public Tools::MessageQueue<Q1>
{
    Q1() : Tools::MessageQueue<Q1>(this, 1) {}

    TOOLS_MQ_CBWRAP(void, Q1, print, void(void))
    {
        std::cout << "printing.\n";
    }

    TOOLS_MQ_CBWRAP(int, Q1, getInt, void(int))
    {
        return 42;
    }

};


struct Q2 : public Tools::MessageQueue<Q2>
{
    Q2() : Tools::MessageQueue<Q2>(this, 1) {}

    TOOLS_MQ_WRAP(void, Q2, print_done)
    {
        std::cout << "print_done.\n";
    }

    TOOLS_MQ_WRAP1(void, Q2, hasInt, int,  i)
    {
        std::cout << "Got int: " << i << std::endl;
    }

    //TOOLS_MQ_WRAP1(double, Q2, hasIntReturnNimp, int, i) // voir explication plus bas
    TOOLS_MQ_WRAP1(void, Q2, hasIntReturnNimp, int, i)
    {
        std::cout << "Got int nimp: " << i << std::endl;
        //return 12;
        // Jérémy: Si je ne me trompe pas, on ne peut jamais récupérer cette valeur; de plus, ça ne fonctionne pas avec l'implementation actuelle (sous vs en tout cas).
    }
};


int main()
{
    Q1 q1;
    Q2 q2;

    Tools::MessageQueueManager::Start(10);

    q1.print(std::bind(&Q2::print_done, &q2));

    q2.hasInt(333);

    q1.getInt(std::bind(&Q2::hasInt, &q2, std::placeholders::_1));

    q2.hasIntReturnNimp(12);

    Tools::Timer::Sleep(1000);
    Tools::MessageQueueManager::Stop();
    return 0;
}
