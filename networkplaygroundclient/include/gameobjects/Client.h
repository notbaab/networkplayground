#ifndef Client_h
#define Client_h
#include "gameobjects/Engine.h"
#include <SDL.h>

class Client : public Engine
{
  public:
    static bool StaticInit();

  protected:
    Client();
    virtual bool DoFrame() override;
    void HandleEvent( SDL_Event* inEvent );
};

#endif /* Client_h */
