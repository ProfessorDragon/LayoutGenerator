#include "GameObjectPool.h"
#include "../PoolObject/PoolObject.h"
#include <random>

const std::vector<PoolObject> GameObjectPool::POOL = []()
{
    std::vector<PoolObject> pool = {};

    // ground
    pool.push_back(
        PoolObject("ground jump")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP)
            .withShares(50.f)
            .withObjectId(-1)
            .withStates(PoolState::GROUNDED, PoolState::NOT_SPIDER)
            .withTap(PoolTap::TAP));

    pool.push_back(
        PoolObject("ground jump robot hold")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP)
            .withShares(25.f)
            .withObjectId(-1)
            .withStates(PoolState::GROUNDED, PoolState::GAMEMODE_ROBOT)
            .withTap(PoolTap::HOLD));

    pool.push_back(
        PoolObject("ground jump robot hold random")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP)
            .withShares(25.f)
            .withObjectId(-1)
            .withStates(PoolState::GROUNDED, PoolState::GAMEMODE_ROBOT)
            .withTap(PoolTap::HOLD_RANDOM));

    pool.push_back(
        PoolObject("ground jump spider")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP | PoolTag::SPIDER | PoolTag::GRAVITY)
            .withShares(50.f)
            .withObjectId(-1)
            .withStates(PoolState::GROUNDED, PoolState::GAMEMODE_SPIDER)
            .withTap(PoolTap::TAP));

    pool.push_back(
        PoolObject("ground fall")
            .withTags(PoolTag::BLOCK | PoolTag::FALL)
            .withShares(12.5f)
            .withObjectId(-1)
            .withStates(PoolState::GROUNDED)
            .withTap(PoolTap::NO));

    pool.push_back(
        PoolObject("ground platform")
            .withTags(PoolTag::BLOCK)
            .withShares(12.5f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::GROUNDED)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::NO)
            .withKeepActive(true));

    // block
    pool.push_back(
        PoolObject("block jump")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP)
            .withShares(25.f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::AIRBORNE, PoolState::FALLING, PoolState::NOT_SPIDER)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::TAP));

    pool.push_back(
        PoolObject("block jump robot hold")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP)
            .withShares(12.5f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::AIRBORNE, PoolState::FALLING, PoolState::GAMEMODE_ROBOT)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::HOLD));

    pool.push_back(
        PoolObject("block jump robot hold random")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP)
            .withShares(12.5f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::AIRBORNE, PoolState::FALLING, PoolState::GAMEMODE_ROBOT)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::HOLD_RANDOM));

    pool.push_back(
        PoolObject("block jump spider")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP | PoolTag::SPIDER | PoolTag::GRAVITY)
            .withShares(25.f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::AIRBORNE, PoolState::FALLING, PoolState::GAMEMODE_SPIDER)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::TAP));

    pool.push_back(
        PoolObject("block fall")
            .withTags(PoolTag::BLOCK | PoolTag::FALL)
            .withShares(12.5f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::AIRBORNE, PoolState::FALLING)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::NO));

    pool.push_back(
        PoolObject("block platform")
            .withTags(PoolTag::BLOCK)
            .withShares(50.f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::AIRBORNE, PoolState::FALLING)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::NO)
            .withKeepActive(true));

    // flying
    pool.push_back(
        PoolObject("flying jump")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP)
            .withShares(25.f)
            .withObjectId(-1)
            .withStates(PoolState::TAP_FLYING)
            .withTap(PoolTap::TAP));

    pool.push_back(
        PoolObject("flying hold")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP)
            .withShares(25.f)
            .withObjectId(-1)
            .withStates(PoolState::HOLD_FLYING)
            .withTap(PoolTap::HOLD));

    pool.push_back(
        PoolObject("flying random")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP)
            .withShares(50.f)
            .withObjectId(-1)
            .withStates(PoolState::HOLD_FLYING)
            .withTap(PoolTap::RANDOM));

    pool.push_back(
        PoolObject("flying fall")
            .withTags(PoolTag::BLOCK | PoolTag::FALL)
            .withShares(50.f)
            .withObjectId(-1)
            .withStates(PoolState::FLYING)
            .withTap(PoolTap::NO));

    pool.push_back(
        PoolObject("flying block fall")
            .withTags(PoolTag::BLOCK | PoolTag::FALL)
            .withShares(12.5f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::FLYING, PoolState::NOT_WAVE, PoolState::FALLING)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::NO));

    pool.push_back(
        PoolObject("flying platform")
            .withTags(PoolTag::BLOCK)
            .withShares(12.5f)
            .withObjectId(ObjectId::BLOCK)
            // bugged specifically with mini wave, hence size_normal|not_wave
            .withStates(PoolState::FLYING, PoolState::FALLING | PoolState::PEAKING, PoolState::SIZE_NORMAL | PoolState::NOT_WAVE)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::NO)
            .withKeepActive(true));

    // pads
    for (auto &p : {
             // yellow
             PartialPoolObject{PoolTag::PAD | PoolTag::JUMP, 35, 5.f},
             // pink
             PartialPoolObject{PoolTag::PAD | PoolTag::JUMP, 140, 2.5f},
             // red
             PartialPoolObject{PoolTag::PAD | PoolTag::JUMP | PoolTag::JUMP_HIGH, 1332},
             // blue
             PartialPoolObject{PoolTag::PAD | PoolTag::JUMP | PoolTag::GRAVITY, 67, 2.5f}})
    {
        pool.push_back(
            PoolObject("pad grounded")
                .withPartial(p)
                .withStates(PoolState::GROUNDED)
                .withAlign(PoolAlign::BR, PoolAlign::BL)
                .withTap(PoolTap::NO));

        pool.push_back(
            PoolObject("pad falling")
                .withPartial(p)
                .withStates(PoolState::AIRBORNE, PoolState::FALLING)
                .withAlign(PoolAlign::BC, PoolAlign::TC)
                .withTap(PoolTap::ANY));
    }

    // rings
    for (auto &p : {
             // yellow
             PartialPoolObject{PoolTag::JUMP, 36, 5.f},
             // pink
             PartialPoolObject{PoolTag::JUMP, 141, 2.5f},
             // red
             PartialPoolObject{PoolTag::JUMP | PoolTag::JUMP_HIGH, 1333},
             // blue
             PartialPoolObject{PoolTag::JUMP | PoolTag::GRAVITY, 84},
             // green
             PartialPoolObject{PoolTag::FALL | PoolTag::GRAVITY, 1022, 2.5f}})
    {
        pool.push_back(
            PoolObject("jump ring grounded")
                .withPartial(p, PoolTag::RING_LATE)
                .withStates(PoolState::GROUNDED)
                .withTap(PoolTap::TAP));

        pool.push_back(
            PoolObject("jump ring falling")
                .withPartial(p, PoolTag::RING_BUFFERED)
                .withStates(PoolState::AIRBORNE, PoolState::FALLING)
                .withAlign(PoolAlign::BC, PoolAlign::TC)
                .withTap(PoolTap::TAP));

        pool.push_back(
            PoolObject("jump ring falling late")
                .withPartial(p, PoolTag::RING_LATE)
                .withStates(PoolState::AIRBORNE, PoolState::FALLING | PoolState::PEAKING)
                .withTap(PoolTap::TAP));

        pool.push_back(
            PoolObject("jump ring hold flying")
                .withPartial(p, PoolTag::RING_LATE)
                .withShares(p.shares * .5f)
                .withStates(PoolState::HOLD_FLYING, PoolState::NOT_WAVE)
                .withTap(PoolTap::TAP));

        pool.push_back(
            PoolObject("jump ring hold flying hold")
                .withPartial(p, PoolTag::RING_LATE)
                .withShares(p.shares * .5f)
                .withStates(PoolState::HOLD_FLYING, PoolState::NOT_WAVE)
                .withTap(PoolTap::HOLD));

        pool.push_back(
            PoolObject("jump ring tap flying")
                .withPartial(p, PoolTag::RING_LATE)
                .withStates(PoolState::TAP_FLYING)
                .withTap(PoolTap::TAP));
    }

    // black rings
    pool.push_back(
        PoolObject("black ring")
            .withTags(PoolTag::RING_BUFFERED | PoolTag::FALL)
            .withShares(.1f)
            .withObjectId(1330)
            .withStates(PoolState::AIRBORNE, PoolState::RISING | PoolState::PEAKING)
            .withAlign(PoolAlign::CR, PoolAlign::CL)
            .withTap(PoolTap::TAP));

    pool.push_back(
        PoolObject("black ring late")
            .withTags(PoolTag::RING_LATE | PoolTag::FALL)
            .withShares(.1f)
            .withObjectId(1330)
            .withStates(PoolState::AIRBORNE, PoolState::RISING | PoolState::PEAKING)
            .withTap(PoolTap::TAP));

    pool.push_back(
        PoolObject("black ring flying")
            .withTags(PoolTag::RING_LATE | PoolTag::FALL)
            .withShares(.1f)
            .withObjectId(1330)
            .withStates(PoolState::FLYING, PoolState::NOT_WAVE)
            .withTap(PoolTap::TAP));

    pool.push_back(
        PoolObject("black ring flying hold")
            .withTags(PoolTag::RING_LATE | PoolTag::FALL)
            .withShares(.1f)
            .withObjectId(1330)
            .withStates(PoolState::HOLD_FLYING, PoolState::NOT_WAVE)
            .withTap(PoolTap::HOLD));

    // spider pads and rings
    for (int i = 0; i < 2; i++)
    {
        int tags = PoolTag::SPIDER;
        if (i == 0)
            tags |= PoolTag::JUMP | PoolTag::GRAVITY;
        else
            tags |= PoolTag::FALL;

        pool.push_back(
            PoolObject("spider pad grounded")
                .withTags(PoolTag::PAD | tags)
                .withShares(.1f)
                .withObjectId(3005)
                .withStates(PoolState::GROUNDED, i == 0 ? PoolState::GRAVITY_NORMAL : PoolState::GRAVITY_REVERSE)
                .withAlign(PoolAlign::BR, PoolAlign::BL)
                .withRotation(i * 180.f)
                .withTap(PoolTap::NO));

        pool.push_back(
            PoolObject("spider pad rising")
                .withTags(PoolTag::PAD | tags)
                .withShares(.1f)
                .withObjectId(3005)
                .withStates(PoolState::AIRBORNE | PoolState::FLYING, PoolState::RISING)
                .withAlign(PoolAlign::TC, PoolAlign::BC)
                .withRotation(i * 180.f)
                .withTap(PoolTap::NO));

        pool.push_back(
            PoolObject("spider pad falling")
                .withTags(PoolTag::PAD | tags)
                .withShares(.1f)
                .withObjectId(3005)
                .withStates(PoolState::AIRBORNE | PoolState::FLYING, PoolState::FALLING)
                .withAlign(PoolAlign::BC, PoolAlign::TC)
                .withRotation(i * 180.f)
                .withTap(PoolTap::NO));

        pool.push_back(
            PoolObject("spider ring")
                .withTags(PoolTag::RING_BUFFERED | tags)
                .withShares(.1f)
                .withObjectId(3004)
                .withStates(PoolState::AIRBORNE)
                .withAlign(PoolAlign::CR, PoolAlign::CL)
                .withRotation(i * 180.f)
                .withTap(PoolTap::TAP));

        pool.push_back(
            PoolObject("spider ring late")
                .withTags(PoolTag::RING_LATE | tags)
                .withShares(.1f)
                .withObjectId(3004)
                .withStates(PoolState::AIRBORNE)
                .withRotation(i * 180.f)
                .withTap(PoolTap::TAP));

        pool.push_back(
            PoolObject("spider ring flying")
                .withTags(PoolTag::RING_LATE | tags)
                .withShares(.1f)
                .withObjectId(3004)
                .withStates(PoolState::FLYING, PoolState::NOT_WAVE)
                .withRotation(i * 180.f)
                .withTap(PoolTap::TAP));
    }

    // dash rings
    // s block: 1829
    for (auto &p : {
             // green
             PartialPoolObject{0, 1704},
             // pink
             PartialPoolObject{PoolTag::GRAVITY, 1751}})
    {
        pool.push_back(
            PoolObject("dash ring falling")
                .withPartial(p, PoolTag::RING_BUFFERED)
                .withShares(.2f)
                .withStates(PoolState::AIRBORNE, PoolState::FALLING)
                .withAlign(PoolAlign::BC, PoolAlign::TC)
                .withTap(PoolTap::HOLD));

        pool.push_back(
            PoolObject("dash ring falling late")
                .withPartial(p, PoolTag::RING_LATE)
                .withShares(.2f)
                .withStates(PoolState::AIRBORNE, PoolState::FALLING | PoolState::PEAKING)
                .withTap(PoolTap::HOLD));

        pool.push_back(
            PoolObject("dash ring flying")
                .withPartial(p, PoolTag::RING_LATE)
                .withShares(.2f)
                // not wave bc it's just not fun
                .withStates(PoolState::FLYING, PoolState::NOT_WAVE)
                .withTap(PoolTap::HOLD));
    }

    // gravity portals
    for (int i = 0; i < 2; i++)
    {
        int state = i == 0 ? PoolState::GRAVITY_REVERSE : PoolState::GRAVITY_NORMAL;

        pool.push_back(
            PoolObject("gravity portal grounded")
                .withTags(PoolTag::PORTAL | PoolTag::GRAVITY)
                .withObjectId(10 + i)
                .withStates(PoolState::GROUNDED, state)
                .withAlign(PoolAlign::BR, PoolAlign::BL)
                .withTap(PoolTap::ANY));

        pool.push_back(
            PoolObject("gravity portal falling")
                .withTags(PoolTag::PORTAL | PoolTag::GRAVITY)
                .withShares(5.f)
                .withObjectId(10 + i)
                .withStates(PoolState::AIRBORNE, PoolState::FALLING, state)
                .withAlign(PoolAlign::BC, PoolAlign::TC)
                .withRotation(90.f)
                .withTap(PoolTap::ANY));

        pool.push_back(
            PoolObject("gravity portal flying")
                .withTags(PoolTag::PORTAL | PoolTag::GRAVITY)
                .withShares(10.f)
                .withObjectId(10 + i)
                .withStates(PoolState::FLYING, state)
                .withAlign(PoolAlign::CR, PoolAlign::CL)
                .withTap(PoolTap::ANY));

        pool.push_back(
            PoolObject("gravity portal jump")
                .withTags(PoolTag::PORTAL | PoolTag::GRAVITY | PoolTag::FALL)
                .withShares(5.f)
                .withObjectId(10 + i)
                .withStates(PoolState::TAP_FLYING, state)
                .withAlign(PoolAlign::CR, PoolAlign::CL)
                .withTap(PoolTap::TAP_DELAYED));
    }

    // size portals
    for (int i = 0; i < 2; i++)
    {
        int state = i == 0 ? PoolState::SIZE_MINI : PoolState::SIZE_NORMAL;

        pool.push_back(
            PoolObject("size portal grounded")
                .withTags(PoolTag::PORTAL | PoolTag::SIZE_)
                .withShares(2.5f)
                .withObjectId(99 + i * 2)
                .withStates(PoolState::GROUNDED, state)
                .withAlign(PoolAlign::BR, PoolAlign::BL)
                .withTap(PoolTap::ANY));

        pool.push_back(
            PoolObject("size portal falling")
                .withTags(PoolTag::PORTAL | PoolTag::SIZE_)
                .withShares(2.5f)
                .withObjectId(99 + i * 2)
                .withStates(PoolState::AIRBORNE, PoolState::FALLING, state)
                .withAlign(PoolAlign::BC, PoolAlign::TC)
                .withRotation(90.f)
                .withTap(PoolTap::ANY));

        pool.push_back(
            PoolObject("size portal flying")
                .withTags(PoolTag::PORTAL | PoolTag::SIZE_)
                .withShares(2.5f)
                .withObjectId(99 + i * 2)
                .withStates(PoolState::FLYING, PoolState::NOT_WAVE, state)
                .withAlign(PoolAlign::CR, PoolAlign::CL)
                .withTap(PoolTap::ANY));

        // increase shares for wave because it's cool
        pool.push_back(
            PoolObject("size portal wave")
                .withTags(PoolTag::PORTAL | PoolTag::SIZE_)
                .withShares(10.f)
                .withObjectId(99 + i * 2)
                .withStates(PoolState::GAMEMODE_WAVE, state)
                .withAlign(PoolAlign::CR, PoolAlign::CL)
                .withTap(PoolTap::ANY));
    }

    // speed 'portals'
    for (int i = 0; i < 5; i++)
    {
        int state = 0;
        if (i != 0)
            state |= PoolState::SPEED_SLOW;
        if (i != 1)
            state |= PoolState::SPEED_NORMAL;
        if (i != 2)
            state |= PoolState::SPEED_2;
        if (i != 3)
            state |= PoolState::SPEED_3;
        if (i != 4)
            state |= PoolState::SPEED_4;

        float shares = std::vector{2.5f, 10.f, 5.f, 2.5f, .5f}[i];

        pool.push_back(
            PoolObject("speed portal grounded")
                .withTags(PoolTag::PORTAL | PoolTag::SPEED)
                .withShares(shares)
                .withObjectId(i == 4 ? 1334 : 200 + i)
                .withStates(PoolState::GROUNDED, state)
                .withAlign(PoolAlign::BR, PoolAlign::BL)
                .withTap(PoolTap::NO));

        pool.push_back(
            PoolObject("speed portal flying")
                .withTags(PoolTag::PORTAL | PoolTag::SPEED)
                .withShares(shares)
                .withObjectId(i == 4 ? 1334 : 200 + i)
                .withStates(PoolState::FLYING, state)
                .withAlign(PoolAlign::CR, PoolAlign::CL)
                .withTap(PoolTap::ANY));
    }

    // gamemode portals
    for (int i = 0; i < 8; i++)
    {
        int state = 0;
        if (i != 0)
            state |= PoolState::GAMEMODE_CUBE;
        if (i != 1)
            state |= PoolState::GAMEMODE_SHIP;
        if (i != 2)
            state |= PoolState::GAMEMODE_BALL;
        if (i != 3)
            state |= PoolState::GAMEMODE_UFO;
        if (i != 4)
            state |= PoolState::GAMEMODE_WAVE; // d block: 1755
        if (i != 5)
            state |= PoolState::GAMEMODE_ROBOT;
        if (i != 6)
            state |= PoolState::GAMEMODE_SPIDER;
        if (i != 7)
            state |= PoolState::GAMEMODE_SWING;

        int objectId = std::vector{12, 13, 47, 111, 660, 745, 1331, 1933}[i];

        pool.push_back(
            PoolObject("gamemode portal grounded")
                .withTags(PoolTag::PORTAL | PoolTag::GAMEMODE)
                .withShares(2.5f)
                .withObjectId(objectId)
                .withStates(PoolState::GROUNDED, state)
                .withAlign(PoolAlign::BR, PoolAlign::BL)
                .withTap(PoolTap::ANY));

        pool.push_back(
            PoolObject("gamemode portal airborne")
                .withTags(PoolTag::PORTAL | PoolTag::GAMEMODE)
                .withShares(2.5f)
                .withObjectId(objectId)
                .withStates(PoolState::AIRBORNE, state)
                .withAlign(PoolAlign::CR, PoolAlign::CL)
                .withTap(PoolTap::ANY));

        pool.push_back(
            PoolObject("gamemode portal flying")
                .withTags(PoolTag::PORTAL | PoolTag::GAMEMODE)
                .withShares(5.f)
                .withObjectId(objectId)
                .withStates(PoolState::FLYING, state)
                .withAlign(PoolAlign::CR, PoolAlign::CL)
                .withTap(PoolTap::ANY));
    }

    return pool;
}();

const PoolObject *GameObjectPool::fish(std::mt19937 &rng, std::function<float(const PoolObject *)> filter)
{
    std::vector<std::tuple<const PoolObject *, float>> filtered;
    float totalShares = 0.f;
    for (auto &fish : POOL)
    {
        float weight = filter(&fish);
        if (weight <= 0.f)
            continue;
        filtered.push_back(std::tuple(&fish, fish.shares * weight));
        totalShares += fish.shares * weight;
        // log::debug("{} {}", fish.name, fish.shares * weight);
    }

    if (filtered.empty())
        return nullptr;

    float n = std::uniform_real_distribution<float>(0, totalShares)(rng);
    for (auto tup : filtered)
    {
        n -= std::get<1>(tup);
        if (n < 0)
            return std::get<0>(tup);
    }

    return std::get<0>(filtered.back());
}
