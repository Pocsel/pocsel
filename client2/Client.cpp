#include <boost/cstdlib.hpp>

#include "client2/Client.hpp"
#include "client2/Settings.hpp"
#include "client2/game/Game.hpp"
#include "client2/resources/LocalResourceManager.hpp"
#include "client2/window/sdl/Window.hpp"
#include "client2/window/InputManager.hpp"
#include "client2/network/Network.hpp"
#include "client2/network/PacketCreator.hpp"
#include "client2/network/PacketDispatcher.hpp"

#include "tools/Timer.hpp"

// XXX EXAMPLE à virer
#include "resources/resources.hpp"
#include "tools/renderers/utils/Font.hpp"
#include "tools/renderers/utils/Image.hpp"
#include "tools/renderers/utils/Rectangle.hpp"
namespace T {
    using Tools::Renderers::Utils::Font;
    using Tools::Renderers::Utils::Image;
    using Tools::Renderers::Utils::Rectangle;
}

namespace {
    T::Font* font;
    T::Image* image;
    Tools::Renderers::ITexture2D* texture;
    T::Rectangle* rectangle;
    Tools::Renderers::IShaderProgram* fontEffect;
    Tools::Renderers::IShaderParameter* fontColor;
    Tools::Renderers::IShaderParameter* fontTexture;
    Tools::Renderers::IShaderProgram* imgEffect;
    Tools::Renderers::IShaderParameter* imgTexture;
    Tools::Renderers::IShaderProgram* rectEffect;
    Tools::Timer totalTimer;
}
// XXX FIN EXAMPLE à virer

namespace Client {

    Client::Client(Settings& settings) :
        _settings(settings),
        _state(Connecting),
        _game(0)
    {
        this->_window = new Window::Sdl::Window(*this);
        this->_resourceManager = new Resources::LocalResourceManager(*this);
        this->_packetDispatcher = new Network::PacketDispatcher(*this);

        this->_window->GetInputManager().GetInputBinder().Bind("eScApE", "quiT");
        this->_window->GetInputManager().Bind(BindAction::Quit, BindAction::Released, std::bind(&Client::Quit, this));

        // XXX EXAMPLE à virer
        font = &this->_resourceManager->GetFont("DejaVuSerif-Italic.ttf", 16);
        image = new T::Image(this->_window->GetRenderer());
        texture = &this->_resourceManager->GetTexture2D("test.png");
        rectangle = new T::Rectangle(this->_window->GetRenderer());
        fontEffect = &this->_resourceManager->GetShader("Fonts.cgfx");
        fontColor = fontEffect->GetParameter("color").release();
        fontTexture = fontEffect->GetParameter("fontTex").release();
        imgEffect = &this->_resourceManager->GetShader("BaseShaderTexture.cgfx");
        imgTexture = imgEffect->GetParameter("baseTex").release();
        rectEffect = &this->_resourceManager->GetShader("BaseShaderColor.cgfx");
        this->_window->GetRenderer().SetClearColor(Tools::Color4f(0.5f, 0.5f, 0.5f, 1));
        this->_window->RegisterCallback(
            [this](Tools::Vector2u const& s)
            {
                this->_window->GetRenderer().SetScreenSize(s);
                this->_window->GetRenderer().SetViewport(Tools::Rectangle(Tools::Vector2i(0), s));
            });
        this->_window->Resize(this->_window->GetSize());
        // XXX FIN EXAMPLE à virer
    }

    Client::~Client()
    {
        Tools::Delete(this->_game);
        Tools::Delete(this->_packetDispatcher);
        Tools::Delete(this->_window);
    }

    int Client::Run()
    {
        Tools::Timer frameTimer;
        this->_network.Connect(this->_settings.host, this->_settings.port);
        this->_network.SendPacket(Network::PacketCreator::Login("yalap_a"));
        while (this->_state)
        {
            frameTimer.Reset();

            if (this->_network.IsConnected())
                this->_packetDispatcher->ProcessAllPackets(this->_network.ProcessInPackets());
            else if (this->_state != Client::Disconnected)
                this->Disconnect("Unknown reason");
            this->_window->GetInputManager().ProcessEvents();
            this->_window->GetInputManager().DispatchActions();

            switch (this->_state)
            {
            case Connecting:
                break;
            case LoadingResources:
                if (this->_game->GetLoadingProgression() == 1.0f)
                {
                    Tools::debug << "LoadingChunks...\n";
                    this->_state = LoadingChunks;
                    this->_network.SendPacket(Network::PacketCreator::Settings(this->_settings));
                }
                break;
            case LoadingChunks:
                break;
            case Running:
                this->_game->Update();
                this->_game->Render();
                break;
            case Disconnected:
                break;
            default:
                ;
            }

            // XXX EXAMPLE à virer
            //this->_window->GetRenderer().Clear(Tools::ClearFlags::Color | Tools::ClearFlags::Depth);
            this->_window->GetRenderer().BeginDraw2D();

            this->_window->GetRenderer().SetModelMatrix(Tools::Matrix4<float>::identity);
            fontColor->Set(Tools::Color4f(1, 1, 1, 1));
            do
            {
                fontEffect->BeginPass();
                font->Render(*fontTexture, "Coucou !");
            } while (fontEffect->EndPass());
            // ----------
            this->_window->GetRenderer().SetModelMatrix(Tools::Matrix4<float>::CreateScale(20, 20, 1) * Tools::Matrix4<float>::CreateTranslation(0, 20, 0));
            do
            {
                rectEffect->BeginPass();
                rectangle->Render();
            } while (rectEffect->EndPass());
            // ----------
            // Attention à l'ordre des multiplications !
            auto tmp = std::sin(0.0005f * totalTimer.GetElapsedTime()) * 20 + 200,
                tmp2 = std::cos(0.001f * totalTimer.GetElapsedTime()) * 50,
                tmp3 = std::sin(0.001f * totalTimer.GetElapsedTime()) * 50;
            this->_window->GetRenderer().SetModelMatrix(
                Tools::Matrix4<float>::CreateYawPitchRollRotation(0, 0, std::sin(0.001f * totalTimer.GetElapsedTime()) * 0.05f)
                * Tools::Matrix4<float>::CreateScale(tmp, tmp, 1)
                * Tools::Matrix4<float>::CreateTranslation(280 + tmp2, 280 + tmp3, 0));
            do
            {
                imgEffect->BeginPass();
                image->Render(*imgTexture, *texture);
            } while (imgEffect->EndPass());

            this->_window->GetRenderer().EndDraw2D();
            // XXX FIN EXAMPLE à virer

            this->_window->Render();

            int timeLeft = 1000 / this->_settings.fps - frameTimer.GetElapsedTime();
            if (timeLeft > 2)
                frameTimer.Sleep(timeLeft);
        }
        this->_network.Stop();
        return boost::exit_success;
    }

    void Client::Login(std::string const& worldIdentifier, std::string const& worldName, Uint32 worldVersion, Common::BaseChunk::CubeType nbCubeTypes)
    {
        if (this->_state != Connecting)
            throw std::runtime_error("Bad client state");
        this->_state = LoadingResources;
        this->_game = new Game::Game(*this, worldIdentifier, worldName, worldVersion, nbCubeTypes);
    }

    void Client::Disconnect(std::string const& reason)
    {
        this->_network.Stop();
        this->_state = Disconnected;
        delete this->_game;
    }

    void Client::Quit()
    {
        this->_state = Quitting;
    }
}
