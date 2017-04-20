#include "gameobjects/Client.h"
#include "gameobjects/GameObject.h"
#include "graphics/GraphicsDriver.h"
#include "graphics/RenderManager.h"
#include "graphics/SpriteComponent.h"
#include "graphics/WindowManager.h"
#include "networking/Logger.h"
#include <iostream>

const char** __argv;
int __argc;

GameObject* test;
SpriteComponent* y;
// init global shit
//void fake_init()
//{
//
//    if ( WindowManager::StaticInit() == false )
//    {
//        std::cout << "WRONG!";
//    }
//
//    if ( GraphicsDriver::StaticInit(
//             WindowManager::sInstance->GetMainWindow() ) == false )
//    {
//        std::cout << "MORE WRONG!";
//    }
//    RenderManager::StaticInit();
//
//    test = new GameObject();
//    test->mLocation.mX = 100;
//    y = new SpriteComponent( test );
//    RenderManager::sInstance->AddComponent( y );
//}

int main( int argc, const char* argv[] )
{

    __argc = argc;
    __argv = argv;

    Logger::InitLog("/Users/erikparreira/Dropbox/Programming/"
                            "networkplayground/client_log.txt");
    if ( Client::StaticInit() )
    {
        return Client::sInstance->Run();
    }
    else
    {
        SDL_Quit();
        // error
        return 1;
    }

    return 0;
}
