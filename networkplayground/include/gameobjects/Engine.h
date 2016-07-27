//
//  Engine.h
//  networkplayground
//
//  Created by Erik Parreira on 7/9/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef Engine_h
#define Engine_h

#include <memory>

class Engine
{
  public:
    virtual ~Engine();
    static std::unique_ptr<Engine> sInstance;

    virtual int Run();

    //    virtual void HandleEvent( SDL_)

  protected:
    Engine();

    virtual void DoFrame();

  private:
    int DoRunLoop();
};

#endif /* Engine_h */
