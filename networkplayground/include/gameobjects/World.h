//
//  Header.h
//  networkplayground
//
//  Created by Erik Parreira on 7/9/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef World_h
#define World_h

#include <memory>
#include <vector>

#include "GameObject.h"

class World
{
  public:
    static void StaticInit();

    static std::unique_ptr<World> sInstance;

    void AddGameObject( GameObjectPtr inGameObject );
    void RemoveGameObject( GameObjectPtr inGameObject );

    void Update();

    const std::vector<GameObjectPtr>& GetGameObjects();

  private:
    World();

    int GetIndexOfGameObject( GameObjectPtr inGameObject );
    std::vector<GameObjectPtr> mGameObjects;
};

#endif /* Header_h */
