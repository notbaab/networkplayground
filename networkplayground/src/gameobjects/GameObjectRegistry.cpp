//
//  GameObjectRegistry.cpp
//  networkplayground
//
//  Created by Erik Parreira on 7/7/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#include "gameobjects/GameObjectRegistry.h"

std::unique_ptr<GameObjectRegistry> GameObjectRegistry::sInstance;

void GameObjectRegistry::StaticInit(addToWorldFunction inAddToWorld)
{
    // Reset pointer to new instance of game object registry
    sInstance.reset(new GameObjectRegistry());
    sInstance->mAddToWorldFunction = inAddToWorld;
}

GameObjectRegistry::GameObjectRegistry() {}
GameObjectPtr GameObjectRegistry::CreateGameObject(uint32_t inCCName)
{
    GameObjectPtr newObject = sInstance->mNameToCreationFunction[inCCName]();

    sInstance->mAddToWorldFunction(newObject);

    return newObject;
}

void GameObjectRegistry::RegisterCreationFunction(uint32_t inCCName,
                                                  GameObjectCreationFunc inCreationFuntion)
{
    sInstance->mNameToCreationFunction[inCCName] = inCreationFuntion;
}
