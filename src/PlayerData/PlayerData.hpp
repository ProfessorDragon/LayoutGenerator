#pragma once

#include <Geode/Geode.hpp>
#include "../PoolObject/PoolEnums.hpp"

using namespace geode::prelude;

class MyPlayerObject;
class PoolObject;

struct PlayerData
{
    MyPlayerObject *player = nullptr;

    CCPoint pos;

    CCPoint velUnscaled;

    CCPoint velScaled;

    PoolState gamemode = PoolState::NONE;

    int state = 0;

    void setPlayer(PlayerObject *player);

    void storeGamemode();

    void storeState();

    bool isClicking() const;

    bool isUpsideDown() const;

    CCSize getRectSize() const;

    int getSign() const;
};

struct PlayerTrailData
{
    CCPoint pos;

    CCPoint velUnscaled;

    bool isClicking = false;

    CCSize rectSize;

    int state = 0;

    // not obtainable from PlayerData

    float boundsCeil = 0.f;

    float boundsFloor = 0.f;

    bool makeJumpIndicator = false;

    // set later in the LayoutGeneratorLayer

    const PoolObject *fish = nullptr;

    // constructor

    static PlayerTrailData fromPlayerData(const PlayerData &pd)
    {
        return PlayerTrailData{
            pd.pos,
            pd.velUnscaled,
            pd.isClicking(),
            pd.getRectSize(),
            pd.state};
    };
};
