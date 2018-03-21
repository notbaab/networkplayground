//
// Created by Erik Parreira on 9/26/16.
//

#include "gameobjects/Ghost.h"
#include "graphics/SpriteComponent.h"
#include "networking/Logger.h"

Ghost::Ghost()
{
    mSpriteComponent.reset( new SpriteComponent( this ) );
    mSpriteComponent->SetColor(new Vector3(0, 0, 255));
}

void Ghost::PrintInfo()
{
   TRACE("Ghost At {}, {}", GetLocation().mX, GetLocation().mY);
}
