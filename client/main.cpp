
#include "tools/gui/Window.hpp"

#include "Application.hpp"

using namespace Client;

int main(int ac, char *av[])
{
    Tools::Gui::Window window("Pocsel");
    Application app(window);

    app.Run(ac, av);

    return 0;
}
