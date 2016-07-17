//
//  GameObjectRegistry.h
//  networkplayground
//
//  Created by Erik Parreira on 7/7/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef GameObjectRegistry_h
#define GameObjectRegistry_h

#include <memory>
#include <stdio.h>
#include <unordered_map>

#include "gameobjects/GameObject.h"

typedef GameObjectPtr ( *GameObjectCreationFunc )();

typedef void ( *addToWorldFunction )( GameObjectPtr );

class GameObjectRegistry
{
  public:
    // Takes a function that should be called after the object is created
    static void StaticInit( addToWorldFunction worldAddFunction );

    // Singleton instance
    static std::unique_ptr<GameObjectRegistry> sInstance;

    void RegisterCreationFunction( uint32_t inCCName,
                                   GameObjectCreationFunc inCreationFuntion );

    GameObjectPtr CreateGameObject( uint32_t inCCName );

  private:
    GameObjectRegistry();

    addToWorldFunction mAddToWorldFunction;

    std::unordered_map<uint32_t, GameObjectCreationFunc>
        mNameToCreationFunction;
};

#endif /* GameObjectRegistry_h */
