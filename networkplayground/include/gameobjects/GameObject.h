//
//  GameObject.h
//  networkplayground
//
//  Created by Erik Parreira on 7/2/16.
//  Copyright © 2016 Erik Parreira. All rights reserved.
//

#ifndef GameObject_h
#define GameObject_h

#include <stdio.h>
#include <cstdint>
#include <memory>


#include "math/Vector3.h"
#include "IO/MemoryBitStream.h"

// Define for identifiing classes.
#define CLASS_IDENTIFICATION( inCode, inClass) \
enum { kClassId = inCode }; \
virtual uint32_t GetClassId() const { return kClassId; } \
static GameObject* CreateInstance() { return static_cast< GameObject* >( new inClass() ); } \


class GameObject
{
public:
    CLASS_IDENTIFICATION('GOBJ', GameObject)
    
    GameObject();
    virtual ~GameObject() {}
    
    virtual void Update();
    
    void SetIndexInWorld( int inIndex ) { mIndexInWorld = inIndex; }
    int GetIndexInWorld() const { return mIndexInWorld; }
    
    int GetNetworkId() const { return mNetworkId; }
    
    virtual uint32_t Write( OutputMemoryBitStream& inOutputStream,
                            uint32_t inDirtyState ) const { return 0; }
    virtual void Read( InputMemoryBitStream& inInputStream ) {}
    
    
    const Vector3& GetLocation() const { return mLocation; }
    
    
private:
    Vector3 mLocation;
    
    int mIndexInWorld;
    int mNetworkId;
    
};

typedef std::shared_ptr< GameObject >	GameObjectPtr;
#endif /* GameObject_h */