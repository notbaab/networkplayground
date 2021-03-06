#include "gameobjects/Client.h"
#include "gameobjects/GameObjectRegistry.h"
#include "gameobjects/PlayerClient.h"
#include "graphics/GraphicsDriver.h"
#include "graphics/RenderManager.h"
#include "graphics/TextureManager.h"
#include "graphics/WindowManager.h"
#include "input/InputManager.h"
#include "networking/Logger.h"
#include "networking/NetworkManagerClient.h"
#include "networking/SocketAddressFactory.h"
#include <gameobjects/World.h>

bool Client::StaticInit()
{
    Client* client = new Client();

    SDL_Init(SDL_INIT_EVERYTHING);

    if (!WindowManager::StaticInit())
    {
        return false;
    }

    if (!GraphicsDriver::StaticInit(WindowManager::sInstance->GetMainWindow()))
    {
        return false;
    }

    TextureManager::StaticInit();
    RenderManager::StaticInit();
    InputManager::StaticInit();
    sInstance.reset(client);

    return true;
}

Client::Client()
{
    std::string destination = "127.0.0.1:45000";
    // std::string name = Logger::GetCommandLineArg( 2 );
    SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString(destination);

    NetworkManagerClient::StaticInit(*serverAddress, "DUDUD");

    GameObjectRegistry::sInstance->RegisterCreationFunction(Player::kClassId,
                                                            PlayerClient::StaticCreate);
}

bool Client::DoFrame()
{
    // Main message loop
    SDL_Event event;
    memset(&event, 0, sizeof(SDL_Event));
    if (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            SDL_Quit();
            return false;
        }
        else
        {
            HandleEvent(&event);
        }
    }

    Timing::sInstance.Update();
    InputManager::sInstance->Update();

    NetworkManagerClient::sInstance->ProcessIncomingPackets();
    Engine::DoFrame();
    RenderManager::sInstance->Render();

    NetworkManagerClient::sInstance->SendOutgoingPackets();

    // Return keep running
    return true;
}

void Client::HandleEvent(SDL_Event* inEvent)
{
    switch (inEvent->type)
    {
    case SDL_KEYDOWN:
        InputManager::sInstance->HandleInput(EIA_Pressed, inEvent->key.keysym.sym);
        break;
    case SDL_KEYUP:
        InputManager::sInstance->HandleInput(EIA_Released, inEvent->key.keysym.sym);
        break;
    default:
        break;
    }
}
