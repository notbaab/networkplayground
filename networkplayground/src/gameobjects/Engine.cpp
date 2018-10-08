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
#include "networking/Logger.h"
#include "networking/SocketUtil.h"

std::unique_ptr<Engine> Engine::sInstance;

Engine::Engine() : running(false)
{
    SocketUtil::StaticInit();
    World::StaticInit();
    GameObjectRegistry::StaticInit(World::StaticAddGameObject);
    //    GameObjectRegistry::StaticInit(<#addToWorldFunction

    //    worldAddFunction#>)
}

Engine::~Engine() { SocketUtil::CleanUp(); }

int Engine::Run()
{
    running = true;
    return DoRunLoop();
}

void Engine::Stop() { running = false; }

int Engine::DoRunLoop()
{
    while (DoFrame() && running)
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
