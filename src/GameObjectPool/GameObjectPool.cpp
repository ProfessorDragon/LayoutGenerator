#include "GameObjectPool.hpp"

#include <random>

#include "../PoolObject/PoolEnums.hpp"
#include "../PoolObject/PoolObject.hpp"

std::vector<PoolObject> GameObjectPool::pool = {};

const float GameObjectPool::BLOCK_SHARES = 50.f;

const float GameObjectPool::VERY_SMALL_SHARES = .05f;

void GameObjectPool::generatePool(std::unordered_map<std::string, uint32_t> customObjects) {
    // ground
    pool.push_back(PoolObject("ground jump")
                       .withTags(PoolTag::BLOCK | PoolTag::JUMP)
                       .withShares(BLOCK_SHARES)
                       .withObjectId(-1)
                       .withStates(PoolState::GROUNDED, PoolState::NOT_ROBOT, PoolState::NOT_SPIDER)
                       .withTap(PoolTap::TAP));

    pool.push_back(PoolObject("ground jump robot")
                       .withTags(PoolTag::BLOCK | PoolTag::JUMP)
                       .withShares(BLOCK_SHARES)
                       .withObjectId(-1)
                       .withStates(PoolState::GROUNDED, PoolState::GAMEMODE_ROBOT)
                       .withTap(PoolTap::HOLD_RANDOM));

    pool.push_back(
        PoolObject("ground jump spider")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP | PoolTag::SPIDER | PoolTag::GRAVITY)
            .withShares(BLOCK_SHARES)
            .withObjectId(-1)
            .withStates(PoolState::GROUNDED, PoolState::GAMEMODE_SPIDER)
            .withTap(PoolTap::TAP));

    pool.push_back(PoolObject("ground fall")
                       .withTags(PoolTag::BLOCK | PoolTag::FALL)
                       .withShares(BLOCK_SHARES / 4.f)
                       .withObjectId(-1)
                       .withStates(PoolState::GROUNDED)
                       .withTap(PoolTap::NO));

    pool.push_back(PoolObject("ground platform")
                       .withTags(PoolTag::BLOCK)
                       .withShares(BLOCK_SHARES / 4.f)
                       .withObjectId(ObjectId::BLOCK)
                       .withStates(PoolState::GROUNDED)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::NO)
                       .withKeepActive(true));

    // block
    pool.push_back(PoolObject("block jump")
                       .withTags(PoolTag::BLOCK | PoolTag::JUMP)
                       .withShares(BLOCK_SHARES / 2.f)
                       .withObjectId(ObjectId::BLOCK)
                       .withStates(PoolState::AIRBORNE, PoolState::FALLING, PoolState::NOT_ROBOT,
                                   PoolState::NOT_SPIDER)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::TAP));

    pool.push_back(
        PoolObject("block jump robot")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP)
            .withShares(BLOCK_SHARES / 2.f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::AIRBORNE, PoolState::FALLING, PoolState::GAMEMODE_ROBOT)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::HOLD_RANDOM));

    pool.push_back(
        PoolObject("block jump spider")
            .withTags(PoolTag::BLOCK | PoolTag::JUMP | PoolTag::SPIDER | PoolTag::GRAVITY)
            .withShares(BLOCK_SHARES / 2.f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::AIRBORNE, PoolState::FALLING, PoolState::GAMEMODE_SPIDER)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::TAP));

    pool.push_back(PoolObject("block fall")
                       .withTags(PoolTag::BLOCK | PoolTag::FALL)
                       .withShares(BLOCK_SHARES / 4.f)
                       .withObjectId(ObjectId::BLOCK)
                       .withStates(PoolState::AIRBORNE, PoolState::FALLING)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::NO));

    pool.push_back(PoolObject("block platform peaking")
                       .withTags(PoolTag::BLOCK)
                       .withShares(BLOCK_SHARES / 4.f)
                       .withObjectId(ObjectId::BLOCK)
                       .withStates(PoolState::AIRBORNE, PoolState::PEAKING)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::NO)
                       .withKeepActive(true));

    pool.push_back(PoolObject("block platform falling")
                       .withTags(PoolTag::BLOCK)
                       .withShares(BLOCK_SHARES / 2.f)
                       .withObjectId(ObjectId::BLOCK)
                       .withStates(PoolState::AIRBORNE, PoolState::FALLING)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::NO)
                       .withKeepActive(true));

    // flying
    pool.push_back(PoolObject("flying jump")
                       .withTags(PoolTag::BLOCK | PoolTag::JUMP)
                       .withShares(BLOCK_SHARES / 2.f)
                       .withObjectId(-1)
                       .withStates(PoolState::TAP_FLYING)
                       .withTap(PoolTap::TAP));

    pool.push_back(PoolObject("flying hold")
                       .withTags(PoolTag::BLOCK | PoolTag::JUMP)
                       .withShares(BLOCK_SHARES / 2.f)
                       .withObjectId(-1)
                       .withStates(PoolState::HOLD_FLYING)
                       .withTap(PoolTap::HOLD));

    pool.push_back(PoolObject("flying fall")
                       .withTags(PoolTag::BLOCK | PoolTag::FALL)
                       .withShares(BLOCK_SHARES)
                       .withObjectId(-1)
                       .withStates(PoolState::FLYING)
                       .withTap(PoolTap::NO));

    pool.push_back(PoolObject("flying block fall")
                       .withTags(PoolTag::BLOCK | PoolTag::FALL)
                       .withShares(BLOCK_SHARES / 2.f)
                       .withObjectId(ObjectId::BLOCK)
                       .withStates(PoolState::FLYING, PoolState::NOT_WAVE, PoolState::FALLING)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::NO));

    pool.push_back(PoolObject("tap flying platform")
                       .withTags(PoolTag::BLOCK)
                       .withShares(BLOCK_SHARES / 2.f)
                       .withObjectId(ObjectId::BLOCK)
                       .withStates(PoolState::TAP_FLYING, PoolState::FALLING | PoolState::PEAKING)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::NO)
                       .withKeepActive(true));

    pool.push_back(
        PoolObject("hold flying platform")
            .withTags(PoolTag::BLOCK)
            .withShares(2.5f)
            .withObjectId(ObjectId::BLOCK)
            // bugged specifically with mini wave, hence size_normal|not_wave
            .withStates(PoolState::HOLD_FLYING, PoolState::FALLING | PoolState::PEAKING,
                        PoolState::SIZE_NORMAL | PoolState::SIZE_BIG | PoolState::NOT_WAVE)
            .withAlign(PoolAlign::BC, PoolAlign::TC)
            .withTap(PoolTap::NO)
            .withKeepActive(true));

    pool.push_back(
        PoolObject("hold flying platform hold")
            .withTags(PoolTag::BLOCK)
            // this appears a lot more than hold flying platform and i don't know why
            // .withShares(2.5f)
            .withObjectId(ObjectId::BLOCK)
            .withStates(PoolState::HOLD_FLYING, PoolState::RISING | PoolState::PEAKING,
                        PoolState::SIZE_NORMAL | PoolState::SIZE_BIG | PoolState::NOT_WAVE)
            .withAlign(PoolAlign::TC, PoolAlign::BC)
            .withTap(PoolTap::HOLD)
            .withKeepActive(true));

    pool.push_back(PoolObject("ship random")
                       .withTags(PoolTag::BLOCK | PoolTag::JUMP)
                       .withShares(BLOCK_SHARES / 2.f)
                       .withObjectId(-1)
                       .withStates(PoolState::GAMEMODE_SHIP)
                       .withTap(PoolTap::RANDOM));

    pool.push_back(PoolObject("ship towards center")
                       .withTags(PoolTag::BLOCK | PoolTag::JUMP)
                       .withShares(BLOCK_SHARES)
                       .withObjectId(-1)
                       .withStates(PoolState::GAMEMODE_SHIP, PoolState::CAMERA_NOT_FREE)
                       .withTap(PoolTap::TOWARDS_CENTER));

    pool.push_back(PoolObject("wave random")
                       .withTags(PoolTag::BLOCK | PoolTag::JUMP)
                       .withShares(BLOCK_SHARES / 4.f)
                       .withObjectId(-1)
                       .withStates(PoolState::GAMEMODE_WAVE)
                       .withTap(PoolTap::RANDOM));

    pool.push_back(PoolObject("wave towards center")
                       .withTags(PoolTag::BLOCK | PoolTag::JUMP)
                       .withShares(BLOCK_SHARES / 2.f)
                       .withObjectId(-1)
                       .withStates(PoolState::GAMEMODE_WAVE, PoolState::CAMERA_NOT_FREE)
                       .withTap(PoolTap::TOWARDS_CENTER));

    // breakable blocks
    pool.push_back(PoolObject("breakable block grounded")
                       .withTags(PoolTag::BREAKABLE_BLOCK)
                       .withShares(VERY_SMALL_SHARES * 2.f)
                       .withObjectId(143)
                       .withStates(PoolState::GROUNDED)
                       .withAlign(PoolAlign::CR, PoolAlign::CL)
                       .withTap(PoolTap::NO));

    pool.push_back(PoolObject("breakable block peaking")
                       .withTags(PoolTag::BREAKABLE_BLOCK)
                       .withShares(VERY_SMALL_SHARES * 2.f)
                       .withObjectId(143)
                       .withStates(PoolState::AIRBORNE, PoolState::PEAKING)
                       .withAlign(PoolAlign::CR, PoolAlign::CL)
                       .withTap(PoolTap::ANY));

    // pads
    // yellow
    addPad(PoolTag::PAD | PoolTag::JUMP, 5.f, 35);
    // pink
    addPad(PoolTag::PAD | PoolTag::JUMP, 2.5f, 140);
    // red
    addPad(PoolTag::PAD | PoolTag::JUMP | PoolTag::JUMP_HIGH, 1.f, 1332);
    // blue
    addPad(PoolTag::PAD | PoolTag::JUMP | PoolTag::GRAVITY, 2.5f, 67);

    // rings
    // yellow
    addRing(PoolTag::JUMP, 5.f, 36);
    // pink
    addRing(PoolTag::JUMP, 2.5f, 141);
    // red
    addRing(PoolTag::JUMP | PoolTag::JUMP_HIGH, 1.f, 1333);
    // blue
    addRing(PoolTag::JUMP | PoolTag::GRAVITY, 1.f, 84);
    // green
    addRing(PoolTag::FALL | PoolTag::GRAVITY, 2.5f, 1022);

    pool.push_back(
        PoolObject("blue ring wave")
            .withTags(PoolTag::JUMP | PoolTag::GRAVITY | PoolTag::RING_LATE | PoolTag::EXPERIMENTAL)
            .withShares(VERY_SMALL_SHARES)
            .withObjectId(84)
            .withStates(PoolState::GAMEMODE_WAVE)
            .withTap(PoolTap::TAP));

    pool.push_back(
        PoolObject("blue ring wave hold")
            .withTags(PoolTag::FALL | PoolTag::GRAVITY | PoolTag::RING_LATE | PoolTag::EXPERIMENTAL)
            .withShares(VERY_SMALL_SHARES)
            .withObjectId(84)
            .withStates(PoolState::GAMEMODE_WAVE)
            .withTap(PoolTap::HOLD));

    // black rings
    pool.push_back(PoolObject("black ring")
                       .withTags(PoolTag::RING_BUFFERED | PoolTag::FALL)
                       .withShares(VERY_SMALL_SHARES)
                       .withObjectId(1330)
                       .withStates(PoolState::AIRBORNE, PoolState::RISING | PoolState::PEAKING)
                       .withAlign(PoolAlign::CR, PoolAlign::CL)
                       .withTap(PoolTap::TAP));

    pool.push_back(PoolObject("black ring late")
                       .withTags(PoolTag::RING_LATE | PoolTag::FALL | PoolTag::EXPERIMENTAL)
                       .withShares(VERY_SMALL_SHARES)
                       .withObjectId(1330)
                       .withStates(PoolState::AIRBORNE, PoolState::RISING | PoolState::PEAKING)
                       .withTap(PoolTap::TAP));

    pool.push_back(PoolObject("black ring hold flying")
                       .withTags(PoolTag::RING_LATE | PoolTag::FALL)
                       .withShares(VERY_SMALL_SHARES)
                       .withObjectId(1330)
                       .withStates(PoolState::HOLD_FLYING, PoolState::NOT_WAVE)
                       .withTap(PoolTap::HOLD));

    pool.push_back(PoolObject("black ring tap flying")
                       .withTags(PoolTag::RING_LATE | PoolTag::FALL)
                       .withShares(VERY_SMALL_SHARES)
                       .withObjectId(1330)
                       .withStates(PoolState::TAP_FLYING, PoolState::NOT_WAVE)
                       .withTap(PoolTap::TAP));

    // spider pads and rings
    for (int i = 0; i < 2; i++) {
        int tags = PoolTag::SPIDER;
        if (i == 0) {
            tags |= PoolTag::JUMP | PoolTag::GRAVITY;
        } else {
            tags |= PoolTag::FALL;
        }

        pool.push_back(PoolObject("spider pad grounded")
                           .withTags(PoolTag::PAD | tags)
                           .withShares(VERY_SMALL_SHARES)
                           .withObjectId(3005)
                           .withStates(PoolState::GROUNDED, i == 0 ? PoolState::GRAVITY_NORMAL
                                                                   : PoolState::GRAVITY_REVERSE)
                           .withAlign(PoolAlign::BR, PoolAlign::BL)
                           .withRotation(i * 180.f)
                           .withTap(PoolTap::NO));

        pool.push_back(PoolObject("spider pad rising")
                           .withTags(PoolTag::PAD | tags)
                           .withShares(VERY_SMALL_SHARES)
                           .withObjectId(3005)
                           .withStates(PoolState::AIRBORNE | PoolState::FLYING, PoolState::RISING)
                           .withAlign(PoolAlign::TC, PoolAlign::BC)
                           .withRotation(i * 180.f)
                           .withTap(PoolTap::NO));

        pool.push_back(PoolObject("spider pad falling")
                           .withTags(PoolTag::PAD | tags)
                           .withShares(VERY_SMALL_SHARES)
                           .withObjectId(3005)
                           .withStates(PoolState::AIRBORNE | PoolState::FLYING, PoolState::FALLING)
                           .withAlign(PoolAlign::BC, PoolAlign::TC)
                           .withRotation(i * 180.f)
                           .withTap(PoolTap::NO));

        pool.push_back(PoolObject("spider ring")
                           .withTags(PoolTag::RING_BUFFERED | tags)
                           .withShares(VERY_SMALL_SHARES)
                           .withObjectId(3004)
                           .withStates(PoolState::AIRBORNE)
                           .withAlign(PoolAlign::CR, PoolAlign::CL)
                           .withRotation(i * 180.f)
                           .withTap(PoolTap::TAP));

        pool.push_back(PoolObject("spider ring late")
                           .withTags(PoolTag::RING_LATE | PoolTag::EXPERIMENTAL | tags)
                           .withShares(VERY_SMALL_SHARES)
                           .withObjectId(3004)
                           .withStates(PoolState::AIRBORNE)
                           .withRotation(i * 180.f)
                           .withTap(PoolTap::TAP));

        pool.push_back(PoolObject("spider ring flying")
                           .withTags(PoolTag::RING_LATE | tags)
                           .withShares(VERY_SMALL_SHARES)
                           .withObjectId(3004)
                           .withStates(PoolState::FLYING, PoolState::NOT_WAVE)
                           .withRotation(i * 180.f)
                           .withTap(PoolTap::TAP));

        pool.push_back(PoolObject("spider ring wave")
                           .withTags(PoolTag::RING_LATE | PoolTag::EXPERIMENTAL | tags)
                           .withShares(VERY_SMALL_SHARES)
                           .withObjectId(3004)
                           .withStates(PoolState::GAMEMODE_WAVE)
                           .withRotation(i * 180.f)
                           .withTap(PoolTap::TAP));
    }

    // dash rings
    addDashRing(0, VERY_SMALL_SHARES * 2.f, 1704);
    addDashRing(PoolTag::GRAVITY, VERY_SMALL_SHARES * 2.f, 1751);

    // gravity portals
    addGravityPortal(10, PoolState::GRAVITY_NORMAL);
    addGravityPortal(11, PoolState::GRAVITY_REVERSE);
    addGravityPortal(2926, 0);

    // size portals
    addSizePortal(99, PoolState::SIZE_NORMAL);
    addSizePortal(101, PoolState::SIZE_MINI);
    if (customObjects.contains("profdragon.bigportal/big-portal")) {
        log::info("Loading with Big Portal support");
        addSizePortal(customObjects["profdragon.bigportal/big-portal"], PoolState::SIZE_BIG);
    }

    // speed 'portals'
    addSpeedPortal(200, PoolState::SPEED_SLOW, 2.5f);
    addSpeedPortal(201, PoolState::SPEED_NORMAL, 10.f);
    addSpeedPortal(202, PoolState::SPEED_2, 5.f);
    addSpeedPortal(203, PoolState::SPEED_3, 2.5f);
    addSpeedPortal(1334, PoolState::SPEED_4, .5f);

    // gamemode portals
    addGamemodePortal(12, PoolState::GAMEMODE_CUBE);
    addGamemodePortal(13, PoolState::GAMEMODE_SHIP);
    addGamemodePortal(47, PoolState::GAMEMODE_BALL);
    addGamemodePortal(111, PoolState::GAMEMODE_UFO);
    addGamemodePortal(660, PoolState::GAMEMODE_WAVE);
    addGamemodePortal(745, PoolState::GAMEMODE_ROBOT);
    addGamemodePortal(1331, PoolState::GAMEMODE_SPIDER);
    addGamemodePortal(1933, PoolState::GAMEMODE_SWING);
}

void GameObjectPool::addPad(int tags, float shares, int objectId) {
    pool.push_back(PoolObject("pad grounded")
                       .withTags(tags)
                       .withObjectId(objectId)
                       .withShares(shares)
                       .withStates(PoolState::GROUNDED)
                       .withAlign(PoolAlign::BR, PoolAlign::BL)
                       .withTap(PoolTap::NO));

    pool.push_back(PoolObject("pad rising")
                       .withTags(tags)
                       .withObjectId(objectId)
                       .withShares(shares * VERY_SMALL_SHARES)
                       .withStates(PoolState::AIRBORNE, PoolState::RISING)
                       .withAlign(PoolAlign::TC, PoolAlign::BC)
                       .withTap(PoolTap::ANY));

    pool.push_back(PoolObject("pad falling")
                       .withTags(tags)
                       .withObjectId(objectId)
                       .withShares(shares)
                       .withStates(PoolState::AIRBORNE, PoolState::FALLING)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::ANY));

    pool.push_back(PoolObject("pad flying rising")
                       .withTags(tags | PoolTag::EXPERIMENTAL)
                       .withObjectId(objectId)
                       .withShares(shares * 2.f)
                       .withStates(PoolState::FLYING, PoolState::NOT_WAVE, PoolState::RISING)
                       .withAlign(PoolAlign::TC, PoolAlign::BC)
                       .withTap(PoolTap::ANY));

    pool.push_back(PoolObject("pad flying falling")
                       .withTags(tags | PoolTag::EXPERIMENTAL)
                       .withObjectId(objectId)
                       .withShares(shares * 2.f)
                       .withStates(PoolState::FLYING, PoolState::NOT_WAVE, PoolState::FALLING)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::ANY));
}

void GameObjectPool::addRing(int tags, float shares, int objectId) {
    pool.push_back(PoolObject("jump ring grounded")
                       .withTags(tags | PoolTag::RING_LATE | PoolTag::EXPERIMENTAL)
                       .withObjectId(objectId)
                       .withShares(shares)
                       .withStates(PoolState::GROUNDED)
                       .withTap(PoolTap::TAP));

    pool.push_back(PoolObject("jump ring falling")
                       .withTags(tags | PoolTag::RING_BUFFERED)
                       .withObjectId(objectId)
                       .withShares(shares)
                       .withStates(PoolState::AIRBORNE, PoolState::FALLING)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::TAP));

    pool.push_back(PoolObject("jump ring falling late")
                       .withTags(tags | PoolTag::RING_LATE | PoolTag::EXPERIMENTAL)
                       .withObjectId(objectId)
                       .withShares(shares)
                       .withStates(PoolState::AIRBORNE, PoolState::FALLING | PoolState::PEAKING)
                       .withTap(PoolTap::TAP));

    pool.push_back(PoolObject("jump ring hold flying")
                       .withTags(tags | PoolTag::RING_LATE)
                       .withObjectId(objectId)
                       .withShares(shares * .5f)
                       .withStates(PoolState::HOLD_FLYING, PoolState::NOT_WAVE)
                       .withTap(tags & PoolTag::JUMP && tags & PoolTag::GRAVITY ? PoolTap::HOLD
                                                                                : PoolTap::TAP));

    // pink rings should only spawn when falling, rising is redundant
    if (objectId == 141) {
        pool.back().states.push_back(PoolState::FALLING);
    }

    pool.push_back(PoolObject("jump ring hold flying experimental")
                       .withTags(tags | PoolTag::RING_LATE | PoolTag::EXPERIMENTAL)
                       .withObjectId(objectId)
                       .withShares(shares * .5f)
                       .withStates(PoolState::HOLD_FLYING, PoolState::NOT_WAVE)
                       .withTap(tags & PoolTag::JUMP && tags & PoolTag::GRAVITY ? PoolTap::TAP
                                                                                : PoolTap::HOLD));

    pool.push_back(PoolObject("jump ring tap flying")
                       .withTags(tags | PoolTag::RING_LATE)
                       .withObjectId(objectId)
                       .withShares(shares)
                       .withStates(PoolState::TAP_FLYING)
                       .withTap(PoolTap::TAP));
}

void GameObjectPool::addDashRing(int tags, float shares, int objectId) {
    pool.push_back(PoolObject("dash ring falling")
                       .withTags(tags | PoolTag::RING_BUFFERED)
                       .withObjectId(objectId)
                       .withShares(shares)
                       .withStates(PoolState::AIRBORNE, PoolState::FALLING)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withTap(PoolTap::HOLD));

    pool.push_back(PoolObject("dash ring falling late")
                       .withTags(tags | PoolTag::RING_LATE | PoolTag::EXPERIMENTAL)
                       .withObjectId(objectId)
                       .withShares(shares)
                       .withStates(PoolState::AIRBORNE, PoolState::FALLING | PoolState::PEAKING)
                       .withTap(PoolTap::HOLD));

    pool.push_back(PoolObject("dash ring flying")
                       .withTags(tags | PoolTag::RING_LATE)
                       .withObjectId(objectId)
                       .withShares(shares)
                       // not wave bc it's just not fun
                       // not peaking because atp just fly straight
                       .withStates(PoolState::FLYING, PoolState::NOT_WAVE,
                                   PoolState::RISING | PoolState::FALLING)
                       .withTap(PoolTap::HOLD));

    pool.push_back(PoolObject("dash ring wave")
                       .withTags(tags | PoolTag::RING_LATE | PoolTag::EXPERIMENTAL)
                       .withObjectId(objectId)
                       .withShares(shares)
                       .withStates(PoolState::GAMEMODE_WAVE)
                       .withTap(PoolTap::HOLD));
}

void GameObjectPool::addGravityPortal(int objectId, int inverseState) {
    int state = 0;
    if (!(inverseState & PoolState::GRAVITY_NORMAL)) state |= PoolState::GRAVITY_NORMAL;
    if (!(inverseState & PoolState::GRAVITY_REVERSE)) state |= PoolState::GRAVITY_REVERSE;

    pool.push_back(PoolObject("gravity portal grounded")
                       .withTags(PoolTag::PORTAL | PoolTag::GRAVITY)
                       .withObjectId(objectId)
                       .withStates(PoolState::GROUNDED, state)
                       .withAlign(PoolAlign::BR, PoolAlign::BL)
                       .withTap(PoolTap::ANY));

    pool.push_back(PoolObject("gravity portal falling")
                       .withTags(PoolTag::PORTAL | PoolTag::GRAVITY)
                       .withShares(5.f)
                       .withObjectId(objectId)
                       .withStates(PoolState::AIRBORNE, PoolState::FALLING, state)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withRotation(90.f)
                       .withTap(PoolTap::ANY));

    pool.push_back(PoolObject("gravity portal hold flying")
                       .withTags(PoolTag::PORTAL | PoolTag::GRAVITY)
                       .withShares(25.f)
                       .withObjectId(objectId)
                       .withStates(PoolState::HOLD_FLYING, state)
                       .withAlign(PoolAlign::CR, PoolAlign::CL)
                       .withTap(PoolTap::ANY));

    pool.push_back(PoolObject("gravity portal tap flying")
                       .withTags(PoolTag::PORTAL | PoolTag::GRAVITY)
                       .withShares(5.f)
                       .withObjectId(objectId)
                       .withStates(PoolState::TAP_FLYING, state)
                       .withAlign(PoolAlign::CR, PoolAlign::CL)
                       .withTap(PoolTap::ANY));

    pool.push_back(
        PoolObject("gravity portal tap flying jump")
            .withTags(PoolTag::PORTAL | PoolTag::GRAVITY | PoolTag::FALL)
            .withShares(5.f)
            .withObjectId(objectId)
            // not grounded
            .withStates(PoolState::TAP_FLYING, PoolState::RISING | PoolState::FALLING, state)
            .withAlign(PoolAlign::CR, PoolAlign::CL)
            .withTap(PoolTap::TAP_DELAYED));
}

void GameObjectPool::addSizePortal(int objectId, int inverseState) {
    int state = 0;
    if (!(inverseState & PoolState::SIZE_NORMAL)) state |= PoolState::SIZE_NORMAL;
    if (!(inverseState & PoolState::SIZE_MINI)) state |= PoolState::SIZE_MINI;
    if (!(inverseState & PoolState::SIZE_BIG)) state |= PoolState::SIZE_BIG;

    pool.push_back(PoolObject("size portal grounded")
                       .withTags(PoolTag::PORTAL | PoolTag::SIZE_)
                       .withShares(2.5f)
                       .withObjectId(objectId)
                       .withStates(PoolState::GROUNDED, state)
                       .withAlign(PoolAlign::BR, PoolAlign::BL)
                       .withTap(PoolTap::ANY));

    pool.push_back(PoolObject("size portal falling")
                       .withTags(PoolTag::PORTAL | PoolTag::SIZE_)
                       .withShares(2.5f)
                       .withObjectId(objectId)
                       .withStates(PoolState::AIRBORNE, PoolState::FALLING, state)
                       .withAlign(PoolAlign::BC, PoolAlign::TC)
                       .withRotation(90.f)
                       .withTap(PoolTap::ANY));

    pool.push_back(PoolObject("size portal flying")
                       .withTags(PoolTag::PORTAL | PoolTag::SIZE_)
                       .withShares(5.f)
                       .withObjectId(objectId)
                       .withStates(PoolState::FLYING, PoolState::NOT_WAVE, state)
                       .withAlign(PoolAlign::CR, PoolAlign::CL)
                       .withTap(PoolTap::ANY));

    // increase shares for wave because it's cool
    pool.push_back(PoolObject("size portal wave")
                       .withTags(PoolTag::PORTAL | PoolTag::SIZE_)
                       .withShares(12.5f)
                       .withObjectId(objectId)
                       .withStates(PoolState::GAMEMODE_WAVE, state)
                       .withAlign(PoolAlign::CR, PoolAlign::CL)
                       .withTap(PoolTap::ANY));
}

void GameObjectPool::addSpeedPortal(int objectId, int inverseState, float shares) {
    int state = 0;
    if (!(inverseState & PoolState::SPEED_SLOW)) state |= PoolState::SPEED_SLOW;
    if (!(inverseState & PoolState::SPEED_NORMAL)) state |= PoolState::SPEED_NORMAL;
    if (!(inverseState & PoolState::SPEED_2)) state |= PoolState::SPEED_2;
    if (!(inverseState & PoolState::SPEED_3)) state |= PoolState::SPEED_3;
    if (!(inverseState & PoolState::SPEED_4)) state |= PoolState::SPEED_4;

    pool.push_back(PoolObject("speed portal grounded")
                       .withTags(PoolTag::PORTAL | PoolTag::SPEED)
                       .withShares(shares)
                       .withObjectId(objectId)
                       .withStates(PoolState::GROUNDED, state)
                       .withAlign(PoolAlign::BR, PoolAlign::BL)
                       .withTap(PoolTap::NO));

    pool.push_back(PoolObject("speed portal flying")
                       .withTags(PoolTag::PORTAL | PoolTag::SPEED)
                       .withShares(shares * 2.f)
                       .withObjectId(objectId)
                       .withStates(PoolState::FLYING, state)
                       .withAlign(PoolAlign::CR, PoolAlign::CL)
                       .withTap(PoolTap::ANY));
}

void GameObjectPool::addGamemodePortal(int objectId, int inverseState) {
    int state = 0;
    if (!(inverseState & PoolState::GAMEMODE_CUBE)) state |= PoolState::GAMEMODE_CUBE;
    if (!(inverseState & PoolState::GAMEMODE_SHIP)) state |= PoolState::GAMEMODE_SHIP;
    if (!(inverseState & PoolState::GAMEMODE_BALL)) state |= PoolState::GAMEMODE_BALL;
    if (!(inverseState & PoolState::GAMEMODE_UFO)) state |= PoolState::GAMEMODE_UFO;
    if (!(inverseState & PoolState::GAMEMODE_WAVE)) state |= PoolState::GAMEMODE_WAVE;
    if (!(inverseState & PoolState::GAMEMODE_ROBOT)) state |= PoolState::GAMEMODE_ROBOT;
    if (!(inverseState & PoolState::GAMEMODE_SPIDER)) state |= PoolState::GAMEMODE_SPIDER;
    if (!(inverseState & PoolState::GAMEMODE_SWING)) state |= PoolState::GAMEMODE_SWING;

    pool.push_back(PoolObject("gamemode portal grounded")
                       .withTags(PoolTag::PORTAL | PoolTag::GAMEMODE)
                       .withShares(2.5f)
                       .withObjectId(objectId)
                       .withStates(PoolState::GROUNDED, state)
                       .withAlign(PoolAlign::BR, PoolAlign::BL)
                       .withTap(PoolTap::ANY));

    pool.push_back(PoolObject("gamemode portal airborne")
                       .withTags(PoolTag::PORTAL | PoolTag::GAMEMODE)
                       .withShares(2.5f)
                       .withObjectId(objectId)
                       .withStates(PoolState::AIRBORNE, state)
                       .withAlign(PoolAlign::CR, PoolAlign::CL)
                       .withTap(PoolTap::ANY));

    pool.push_back(PoolObject("gamemode portal flying")
                       .withTags(PoolTag::PORTAL | PoolTag::GAMEMODE)
                       .withShares(5.f)
                       .withObjectId(objectId)
                       .withStates(PoolState::FLYING, state)
                       .withAlign(PoolAlign::CR, PoolAlign::CL)
                       .withTap(PoolTap::ANY));
}

const PoolObject* GameObjectPool::fish(std::function<float(const PoolObject*)> filter) {
    std::vector<std::tuple<const PoolObject*, float>> filtered;
    float totalShares = 0.f;
    for (auto& fish : pool) {
        float weight = filter(&fish);
        if (weight <= 0.f) {
            continue;
        }
        filtered.push_back(std::tuple(&fish, fish.shares * weight));
        totalShares += fish.shares * weight;
        // log::debug("{} {}", fish.name, fish.shares * weight);
    }

    if (filtered.empty()) {
        return nullptr;
    }

    float n = utils::random::generate<float>(0, totalShares);
    for (auto tup : filtered) {
        n -= std::get<1>(tup);
        if (n < 0) return std::get<0>(tup);
    }

    return std::get<0>(filtered.back());
}
