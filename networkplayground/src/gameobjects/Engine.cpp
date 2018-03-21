//
//  Engine.cpp
//  networkplayground
//
//  Created by Erik Parreira on 7/9/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include "gameobjects/Engine.h"
#include "gameobjects/GameObjectRegistry.h"
#include "gameobjects/World.h"
#include "networking/SocketUtil.h"
#include "networking/Logger.h"

std::unique_ptr<Engine> Engine::sInstance;

Engine::Engine()
{
    SocketUtil::StaticInit();
    World::StaticInit();
    GameObjectRegistry::StaticInit( World::StaticAddGameObject );
    //    GameObjectRegistry::StaticInit(<#addToWorldFunction
    //    worldAddFunction#>)
}

Engine::~Engine()
{
    SocketUtil::CleanUp();

       // SDL_Quit();
}

int Engine::Run() { return DoRunLoop(); }
// void Engine::HandleEvent( SDL_Event* inEvent )
//{
//    // Default implementation does nothing, up to derived classes to handle
//    them, if they so choose
//    ( void )inEvent;
//}

int Engine::DoRunLoop()
{

    while ( DoFrame() )
    {
    }

    INFO("Exiting");
    return 0;
}

bool Engine::DoFrame()
{
    World::sInstance->Update();
    return true;
}
