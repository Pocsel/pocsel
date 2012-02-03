#include "client/Application.hpp"

#include "tools/gui/Window.hpp"

using namespace Client;

int main(int ac, char *av[])
{
    Tools::Gui::Window window("Pocsel");
    Application app(window);

    app.Run(ac, av);

    return 0;
}
