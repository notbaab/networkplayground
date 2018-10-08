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

    void AddGameObject(GameObjectPtr inGameObject);
    static void StaticAddGameObject(GameObjectPtr inGameObject);
    static void PrintInfo();

    void RemoveGameObject(GameObjectPtr inGameObject);

    void Update();

    const std::vector<GameObjectPtr>& GetGameObjects();

  private:
    World();

    int GetIndexOfGameObject(GameObjectPtr inGameObject);
    std::vector<GameObjectPtr> mGameObjects;
};

#endif /* Header_h */
