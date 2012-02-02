
#include <boost/thread.hpp>
#include "mapgen/MapGenerator.hpp"

static Server::MapGen::MapGenerator::DoneList done;
static boost::mutex mut;

void PushDone(Server::MapGen::MapGenerator::DoneList& done1)
{
    boost::unique_lock<boost::mutex> lock(mut);
    done.splice(done.end(), done1);
}

int main()
{
    int const toto = 5;
    Server::MapGen::MapGenerator* gen = new Server::MapGen::MapGenerator(&PushDone);

    gen->Run();

    for (int x = 0 ; x < toto ; ++x)
    {
        for (int y = 0 ; y < toto ; ++y)
        {
            Server::MapGen::MapGenerator::TodoList l;
            for (int z = 0 ; z < toto ; ++z)
            {
                l.push_back(Chunk::CoordsType(x, y, z));
            }
            gen->Push(l);
        }
    }

    std::cout << "Loop End\n";

    auto startTime = boost::get_system_time().time_of_day().total_milliseconds();
    mut.lock();
    while (done.size() < toto * toto * toto)
    {
        mut.unlock();
        boost::this_thread::sleep(boost::posix_time::milliseconds(5));
        mut.lock();
    }
    mut.unlock();
    std::cout << "time= " << (boost::get_system_time().time_of_day().total_milliseconds() - startTime) << "\n";

    std::cout << "delete...\n";
    delete gen;
    std::cout << "delete done\n";

    return 0;
}

