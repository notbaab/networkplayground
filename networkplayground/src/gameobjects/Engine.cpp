//
//  Engine.cpp
//  networkplayground
//
//  Created by Erik Parreira on 7/9/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include "gameobjects/Engine.h"
#include "gameobjects/GameObjectRegistry.h"
#include "networking/SocketUtil.h"
#include <memory>
#include <stdio.h>
#include <time.h>

std::unique_ptr<Engine> Engine::sInstance;

Engine::Engine()
{
    SocketUtil::StaticInit();

    //    GameObjectRegistry::StaticInit(<#addToWorldFunction
    //    worldAddFunction#>)
}

Engine::~Engine()
{
    SocketUtil::CleanUp();

    //    SDL_Quit();
}

int Engine::Run() { return DoRunLoop(); }
// void Engine::HandleEvent( SDL_Event* inEvent )
//{
//    // Default implementation does nothing, up to derived classes to handle
//    them, if they so choose
//    ( void )inEvent;
//}

int Engine::DoRunLoop() { return 0; }
void Engine::DoFrame()
{
    //    World::sInstance->Update();
}
