#include "client/Application.hpp"

#include "tools/gui/Window.hpp"

using namespace Client;

int main(int ac, char *av[])
{
    bool useShaders = true;

    if (ac > 1 && std::string(av[1]) == "--no-shaders")
    {
        ++av;
        --ac;
        useShaders = false;
    }
    Tools::Gui::Window window("Pocsel", 640, 480, useShaders);
    Application app(window);

    app.Run(ac, av);

    return 0;
}
