//
// The ghosts of objects. Contains the location of the players that we last know
// from the server USED ONLY FOR READING FROM THE SERVER

#ifndef NETWORKPLAYGROUND_PLAYERGHOST_H_H
#define NETWORKPLAYGROUND_PLAYERGHOST_H_H

#include "gameobjects/GameObject.h"

class SpriteComponent;

class Ghost : public GameObject
{
  public:
    Ghost();
    void PrintInfo();
    // An enum in most implementations but always an uint32 when reading
    // and writing from a stream
    uint32_t mState;

    // Which player does this object belong to. Will this make sense for more
    // objects? I don't know, time will tell.
    uint32_t mPlayerId;

    std::shared_ptr<SpriteComponent> mSpriteComponent;
};

#endif // NETWORKPLAYGROUND_PLAYERGHOST_H_H
