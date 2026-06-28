#include "PlayerData.hpp"
#include "../PlayerObject.cpp"

void PlayerData::setPlayer(PlayerObject *player)
{
    this->player = static_cast<MyPlayerObject *>(player);
    if (!this->player)
        return;

    pos = player->getPosition();
    velUnscaled = CCPoint{(float)player->getCurrentXVelocity(), (float)player->getYVelocity()};
    storeGamemode();
    storeState();
}

void PlayerData::storeGamemode()
{
    if (player->m_isShip)
        gamemode = PoolState::GAMEMODE_SHIP;
    else if (player->m_isBall)
        gamemode = PoolState::GAMEMODE_BALL;
    else if (player->m_isBird)
        gamemode = PoolState::GAMEMODE_UFO;
    else if (player->m_isDart)
        gamemode = PoolState::GAMEMODE_WAVE;
    else if (player->m_isRobot)
        gamemode = PoolState::GAMEMODE_ROBOT;
    else if (player->m_isSpider)
        gamemode = PoolState::GAMEMODE_SPIDER;
    else if (player->m_isSwing)
        gamemode = PoolState::GAMEMODE_SWING;
    else
        gamemode = PoolState::GAMEMODE_CUBE;
}

void PlayerData::storeState()
{
    state = 0;

    float yv = velUnscaled.y * getSign();
    if (yv > 1.f)
        state |= PoolState::RISING;
    else if (yv < -1.f)
        state |= PoolState::FALLING;
    if (abs(yv) < 3)
        state |= PoolState::PEAKING;

    state |= isUpsideDown() ? PoolState::GRAVITY_REVERSE : PoolState::GRAVITY_NORMAL;

    state |= isCameraFree() ? PoolState::CAMERA_FREE : PoolState::CAMERA_NOT_FREE;

    state |= player->m_vehicleSize < 1.f ? PoolState::SIZE_MINI : PoolState::SIZE_NORMAL;

    if (player->m_playerSpeed == 0.7f)
        state |= PoolState::SPEED_SLOW;
    else if (player->m_playerSpeed == 0.9f)
        state |= PoolState::SPEED_NORMAL;
    else if (player->m_playerSpeed == 1.1f)
        state |= PoolState::SPEED_2;
    else if (player->m_playerSpeed == 1.3f)
        state |= PoolState::SPEED_3;
    else if (player->m_playerSpeed == 1.6f)
        state |= PoolState::SPEED_4;

    state |= gamemode;

    // ground detection (doesn't work for flying gamemodes)
    if (state & PoolState::NOT_FLYING)
        state |= player->m_isOnGround ? PoolState::GROUNDED : PoolState::AIRBORNE;
}

bool PlayerData::isCameraFree() const
{
    // see GJBaseGameLayer::updateCameraMode
    // man i wish this was named
    return player->m_gameLayer->m_gameState.m_unkBool8;
}

// m_jumpBuffered seems to be the same as m_holdingButtons[(int)PlayerButton::Jump],
// but is false after hitting a spider pad/ring
bool PlayerData::isClicking() const
{
    return player->m_jumpBuffered;
}

bool PlayerData::isUpsideDown() const
{
    return player->m_isUpsideDown;
}

CCSize PlayerData::getRectSize() const
{
    return player->getObjectRect().size;
}

int PlayerData::getSign() const
{
    return isUpsideDown() ? -1 : 1;
}

bool PlayerTrailData::isCameraFree() const
{
    return state & PoolState::CAMERA_FREE;
}

bool PlayerTrailData::isUpsideDown() const
{
    return state & PoolState::GRAVITY_REVERSE;
}

int PlayerTrailData::getSign() const
{
    return isUpsideDown() ? -1 : 1;
}
