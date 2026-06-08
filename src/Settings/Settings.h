#pragma once
#include "../PoolObject/PoolEnums.h"

class Settings
{
protected:
    float bpm = 128.f;

    int excludeTags = 0; // e.g. PoolTag::GAMEMODE

    const float extraSongOffset = 0.133f; // offset for better sync - experimental

    bool usePlayerClicks = false;

public:
    float getBpm() { return bpm; }

    void setBpm(float bpm) { this->bpm = bpm; }

    int getExcludeTags() { return excludeTags; }

    void setExcludeTags(int excludeTags) { this->excludeTags = excludeTags; }

    float getExtraSongOffset() { return extraSongOffset; }

    bool getUsePlayerClicks() { return usePlayerClicks; }

    bool getUseRandomClicks() { return !usePlayerClicks; }

    void setUsePlayerClicks(bool usePlayerClicks) { this->usePlayerClicks = usePlayerClicks; }
};
