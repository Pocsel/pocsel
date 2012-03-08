#include <boost/thread.hpp>
#include <boost/ref.hpp>
#include <iostream>
#include <utility>
#include <memory>

#include "Assert.hpp"
#include "server/EventManager.hpp"

struct GameTree
{
    std::string name;
    int age;
    int speed;

    GameTree(std::string const& name, int speed) :
        name(name),
        age(0),
        speed(speed)
    {
    }

    ~GameTree()
    {
        std::cout << this->name << " est mort à " << this->age << "." << std::endl;
    }
};

struct GameTreeGrowingEvent
{
private:
    std::weak_ptr<GameTree> _tree;
    Server::EventManager& mgr;

public:
    GameTreeGrowingEvent(Server::EventManager& mgr, std::shared_ptr<GameTree>& tree) :
      _tree(tree),
      mgr(mgr)
    {
    }

    void operator() ()
    {
        // Permet de vérfier et lock le pointeur si jamais l'arbre est supprimé pendant l'execution de l'evenement
        if (std::shared_ptr<GameTree> tree = this->_tree.lock())
        {
            ++tree->age;
            std::cout << tree->name << " a grandit (" << tree->age << ")" << std::endl;

           mgr.AddEvent(tree->speed, GameTreeGrowingEvent(mgr, tree));
        }
    }
};
int main(int, char**)
{
    Server::EventManager mgr;
    std::shared_ptr<GameTree> bite((GameTree*)0);
    GameTreeGrowingEvent a(mgr, bite);
    mgr.AddEvent(10L, a);
    boost::thread t1(boost::bind(&Server::EventManager::Run, &mgr));
    boost::thread t2(boost::bind(&Server::EventManager::Run, &mgr));
    boost::thread t3(boost::bind(&Server::EventManager::Run, &mgr));
    boost::thread t4(boost::bind(&Server::EventManager::Run, &mgr));
    boost::thread t5(boost::bind(&Server::EventManager::Run, &mgr));
    boost::thread t6(boost::bind(&Server::EventManager::Run, &mgr));

    {
        std::shared_ptr<GameTree> c(new GameTree("c", 3000));
    for (int i = 0; i < 100; ++i)
    {
        GameTreeGrowingEvent b(mgr, c);
        mgr.AddEvent(1000L, b);
    }
        boost::this_thread::sleep(boost::posix_time::milliseconds(2));
    }

    //mgr.Run();
    std::cout << "tests done.\n";
    return 0;
}
