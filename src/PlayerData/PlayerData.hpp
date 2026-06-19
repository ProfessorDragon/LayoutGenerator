#pragma once

#include <Geode/Geode.hpp>
#include "../PoolObject/PoolEnums.hpp"

using namespace geode::prelude;

struct PlayerData
{
    PlayerObject *player = nullptr;

    CCPoint pos;

    CCPoint velUnscaled;

    CCPoint velScaled;

    PoolState gamemode = PoolState::NONE;

    int state = 0;

    bool isClicking() { return player->m_holdingButtons[(int)PlayerButton::Jump]; }

    bool isUpsideDown() { return player->m_isUpsideDown; }

    int getSign() { return isUpsideDown() ? -1 : 1; }
};

struct PlayerTrailData
{
    CCPoint pos;

    CCPoint velUnscaled;

    CCPoint velScaled;

    float rectWidth;

    int state;

    float boundsCeil;

    float boundsFloor;

    const PoolObject *fish = nullptr;
};
