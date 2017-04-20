#include "gameobjects/PlayerServer.h"
#include "networking/Logger.h"
#include "const/consts.h"

PlayerServer::PlayerServer() : mControlType( PCT_HUMAN ) {}

void PlayerServer::Update()
{
    Player::Update();

    Vector3 oldLocation = GetLocation();
    Vector3 oldVelocity = GetVelocity();

    if ( mControlType == PCT_HUMAN )
    {
        ClientProxyPtr client =
            NetworkManagerServer::sInstance->GetClientProxy( GetPlayerId() );

        if ( !client )
        {
            LOG( "NO HUMANC CONTROLING THIS, IT HAS BECOME SENTIENT RUN" );
            return;
        }

        MoveList& moveList = client->GetUnprocessedMoveList();
        for ( const Move& unprocessedMove : moveList )
        {
            const InputState& currentState = unprocessedMove.GetInputState();

            float deltaTime = unprocessedMove.GetDeltaTime();
            ProcessInput( deltaTime, currentState );
            SimulateMovement( TIME_STEP );
        }
        moveList.Clear();

        // TODO: Check if there velocity has actually changed before sending
        // update
        NetworkManagerServer::sInstance->SetStateDirty( GetNetworkId(),
                                                        PRS_POSI );
    }
}
