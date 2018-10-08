#include "gameobjects/PlayerServer.h"
#include "const/consts.h"
#include "networking/Logger.h"

PlayerServer::PlayerServer() : mControlType(PCT_HUMAN) {}

void PlayerServer::Update()
{
    Player::Update();

    Vector3 oldLocation = GetLocation();
    Vector3 oldVelocity = GetVelocity();

    if (mControlType == PCT_HUMAN)
    {
        ClientProxyPtr client =
            NetworkManagerServer::sInstance->GetClientProxy(GetPlayerId());

        if (!client)
        {
            CRITICAL("NO HUMAN CONTROLLING THIS, IT HAS BECOME SENTIENT RUN");
            return;
        }

        MoveList& moveList = client->GetUnprocessedMoveList();
        for (const Move& unprocessedMove : moveList)
        {
            const InputState& currentState = unprocessedMove.GetInputState();

            float deltaTime = unprocessedMove.GetDeltaTime();
            ProcessInput(0, currentState);
            SimulateMovement(TIME_STEP);
        }
        moveList.Clear();

        if (Math::Is2DVectorEqual(oldVelocity, GetVelocity()) &&
            Math::Is2DVectorEqual(oldLocation, GetLocation()))
        {
            // if (GetVelocity().mX == 0 || GetVelocity().mY == 0) {
            //     TRACE("{} Not moving", GetNetworkId());
            // }
            // no change, return
            return;
        }

        if (GetVelocity().mX == 0 || GetVelocity().mY == 0)
        {
            ERROR("{} Not moving but updating", GetNetworkId());
        }

        NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(),
                                                       PRS_POSI);
    }
}
