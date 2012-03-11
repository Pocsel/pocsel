#include "tests/database/precompiled.hpp"

#include "Connection.hpp"

int main(int ac, char *av[])
{
    std::unique_ptr<Tools::Database::IConnection> conn(new Tools::Database::Sqlite::Connection("test.sqlite"));

    try
    {
        std::srand(1230); // seed statique
        conn->CreateQuery("DROP TABLE IF EXISTS users")->ExecuteNonSelect();
        conn->CreateQuery("DROP TABLE IF EXISTS stress")->ExecuteNonSelect();

        conn->CreateQuery("CREATE TABLE users (id INTEGER PRIMARY KEY, login TEXT, password TEXT)")->ExecuteNonSelect();
        conn->CreateQuery("CREATE TABLE stress (id INTEGER PRIMARY KEY, size INTEGER, data TEXT)")->ExecuteNonSelect();

        auto query = conn->CreateQuery("INSERT INTO users (login, password) VALUES (?, ?)");
        auto query2 = conn->CreateQuery("INSERT INTO stress (size, data) VALUES (?, ?)");

        conn->BeginTransaction();
        query->Bind("titi").Bind("bite").ExecuteNonSelect().Reset();
        query->Bind("hello").Bind("world").ExecuteNonSelect().Reset();
        query->Bind("root").Bind("pass").ExecuteNonSelect().Reset();
        query->Bind("admin").Bind("1234").ExecuteNonSelect().Reset();
        for (int i = 0; i < 50; ++i)
            query2->Bind(std::rand()).Bind(Tools::ToString(std::rand())).ExecuteNonSelect().Reset();
        conn->EndTransaction();

        auto it = conn->CreateQuery("SELECT id, login, password, (SELECT 1) FROM users");
        while (auto ptr = it->Fetch())
            Tools::log << std::setw(5) << ptr->GetInt(0)  << std::setw(12) << ptr->GetString(1) << std::setw(12) << ptr->GetString(2) << std::setw(5) << ptr->GetInt(3) << std::endl;

        for (int i = 0; i < 50; ++i)
            query2->Bind(std::rand()).Bind(Tools::ToString(std::rand())).ExecuteNonSelect().Reset();
    }
    catch (std::exception& e)
    {
        Tools::error << e.what() << std::endl;
    }
#ifdef _WIN32
    std::cout << "Press enter\n";
    std::cin.get();
#endif
    return 0;
}
