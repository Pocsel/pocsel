
#include <iostream>

#include "tools/database/sqlite/Connection.hpp"

int main(int ac, char* av[])
{
    Tools::Database::Sqlite::Connection conn("db.sqlite3");

    auto& curs = conn.GetCursor();
    try {
        curs.Execute("CREATE TABLE users (id INTEGER PRIMARY KEY, login TEXT, password TEXT)");
    } catch(std::exception&) {
     //   std::cerr << err.what() << std::endl;
    }

    if (ac > 1)
    {
        curs.Execute(av[1]);
        if (ac <= 2)
        {
            std::cout << "noob" << std::endl;
            return 0;
        }
        while (curs.HasData())
        {
            auto& row = curs.FetchOne();
            for (int i = 0; av[2][i]; i++)
            {
                switch(av[2][i])
                {
                case 'i':
                    std::cout << "i:" << row[i].GetInt();
                    break;
                case 'I':
                    std::cout << "I:" << row[i].GetInt64();
                    break;
                case 'u':
                case 'U':
                    std::cout << "U:" << row[i].GetUint64();
                    break;
                case 's':
                case 'S':
                    std::cout << "s:" << row[i].GetString();
                    break;
                case 'f':
                    std::cout << "f:" << row[i].GetFloat();
                    break;
                case 'F':
                case 'd':
                case 'D':
                    std::cout << "d:" << row[i].GetDouble();
                    break;
                }
                std::cout << ' ';
            }
            std::cout << std::endl;
        }
    }
    else
        std::cout << "no args\n";
}


