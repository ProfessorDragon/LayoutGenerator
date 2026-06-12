#pragma once

#include <Geode/Geode.hpp>
#include "PoolEnums.hpp"

using namespace geode::prelude;

struct PartialPoolObject
{
    int tags = 0;

    int objectId = 0;

    float shares = 1.f;
};

struct PoolObject
{
    std::string name;

    int tags = 0;

    float shares = 1.f;

    int objectId = 0;

    std::vector<int> states = {};

    PoolAlign alignPlayer = PoolAlign::CC;

    PoolAlign alignObject = PoolAlign::CC;

    float rotation = 0.f;

    PoolTap tap = PoolTap::NO;

    bool keepActive = false;

    PoolObject(std::string name)
    {
        this->name = name;
    }

    PoolObject &withPartial(PartialPoolObject partial, int extraTags = 0, float shareMultiplier = 1.f)
    {
        tags = partial.tags | extraTags;
        objectId = partial.objectId;
        shares = partial.shares * shareMultiplier;
        return *this;
    }

    PoolObject &withTags(int tags)
    {
        this->tags = tags;
        return *this;
    }

    PoolObject &withShares(float shares)
    {
        this->shares = shares;
        return *this;
    }

    PoolObject &withObjectId(int objectId)
    {
        this->objectId = objectId;
        return *this;
    }

    template <typename... Args>
    PoolObject &withStates(Args &&...states)
    {
        (this->states.push_back(std::forward<Args>(states)), ...);
        return *this;
    }

    PoolObject &withAlign(PoolAlign alignPlayer, PoolAlign alignObject)
    {
        this->alignPlayer = alignPlayer;
        this->alignObject = alignObject;
        return *this;
    }

    PoolObject &withRotation(float rotation)
    {
        this->rotation = rotation;
        return *this;
    }

    PoolObject &withTap(PoolTap tap)
    {
        this->tap = tap;
        return *this;
    }

    PoolObject &withKeepActive(bool keepActive)
    {
        this->keepActive = keepActive;
        return *this;
    }

    bool canFlip() const
    {
        return !(tags & PoolTag::PORTAL);
    }

    bool matchesPlayerState(int state) const
    {
        for (auto requireFlag : states)
        {
            if (!(state & requireFlag))
                return false;
        }
        return true;
    }
};
