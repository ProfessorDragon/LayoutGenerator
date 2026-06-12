#include "LayoutGeneratorLayer.h"
#include "../GameObjectPool/GameObjectPool.h"
#include "../PoolObject/PoolObject.h"
#include "../Settings/Settings.h"
#include "../Settings/SettingsPopup.h"

#define MACRO_PLAYER_DATA                                                                             \
    LevelEditorLayer *editor = LevelEditorLayer::get();                                               \
    PlayerObject *player = editor->m_player1;                                                         \
    CCPoint playerPos = player->getPosition();                                                        \
    CCPoint playerVel = CCPoint{(float)player->getCurrentXVelocity(), (float)player->getYVelocity()}; \
    bool upsideDown = player->m_isUpsideDown;                                                         \
    int sign = upsideDown ? -1 : 1;                                                                   \
    PoolState gamemode = getPlayerGamemode(player);                                                   \
    int state = getPlayerState(player);

Settings *getSettings()
{
    static Settings settings;
    return &settings;
}

LayoutGeneratorLayer *LayoutGeneratorLayer::create()
{
    auto ret = new LayoutGeneratorLayer();
    if (ret->init())
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool LayoutGeneratorLayer::init()
{
    if (!CCLayer::init())
    {
        return false;
    }

    // parse hitboxes file
    // auto path = geode::Mod::get()->getResourcesDir() / "hitboxes.json";
    // std::ifstream file(path);
    // if (!file.is_open())
    // {
    //     log::error("Failed to open file");
    //     return false;
    // }

    // auto result = matjson::parse(file);
    // if (result.isErr())
    // {
    //     log::error("Failed to read JSON");
    //     return false;
    // }

    // auto json = result.unwrap();
    // auto block = json["8"].asArray().unwrap();
    // log::info("{}, {}", block[0].asString(), block[1].asString());

    // for debugging CCRects
    // log::debug("{} {} {} {}", rect.getMinX(), rect.getMinY(), rect.getMaxX(), rect.getMaxY());

    // schedule update
    scheduleUpdate();

    return true;
}

void LayoutGeneratorLayer::reset()
{
    m_isBuilding = true;
    m_boundsCeil = 1300.f;
    m_boundsFloor = 90.f;
    m_canPlaceNextFrame = true;
    m_elapsedTime = LevelEditorLayer::get()->m_levelSettings->m_songOffset + getSettings()->getExtraSongOffset();
    m_hasTappedThisGamemode = false;
    m_isClickingLastFrame = false;
    m_lastGamemodePortalPos = CCPoint{0.f, m_boundsFloor};
    m_lastPlacedFish = nullptr;
    m_lastPlacedFishPos = CCPoint{};
    m_lastPlayerGamemode = PoolState::GAMEMODE_CUBE;
    m_lastSpikeBottomPos = CCPoint{};
    m_lastSpikeTopPos = CCPoint{};
    m_placeAgainTimer = 0;
    m_playerTrail.clear();
    m_shouldTap = PoolTap::NO;
    m_shouldTapTimer = 0;
    m_tapBalance = 3.f; // don't make the player jump straight away
}

void LayoutGeneratorLayer::update(float dt)
{
    CCLayer::update(dt);

    if (!m_isBuilding)
        return;

    MACRO_PLAYER_DATA;

    bool useRandomClicks = getSettings()->getUseRandomClicks();

    // paused
    if (!m_playerTrail.empty() && m_playerTrail.back().pos == playerPos)
        return;
    m_playerTrail.push_back(PlayerTrailData{
        playerPos, playerVel, player->getObjectRect().size.width, state, m_boundsCeil, m_boundsFloor});

    // update gamemode bounds
    if (gamemode != m_lastPlayerGamemode)
    {
        if (gamemode & PoolState::NO_BOUNDS)
        {
            m_boundsFloor = 90.f;
            m_boundsCeil = 1300.f;
        }
        else
        {
            float height;
            if (gamemode == PoolState::GAMEMODE_BALL)
                height = 240.f;
            else if (gamemode == PoolState::GAMEMODE_SPIDER)
                height = 270.f;
            else // ship and ufo
                height = 300.f;
            m_boundsFloor = std::max(90.f, 30.f * std::ceil((player->m_lastPortalPos.y - (height / 2.f + 30.f)) / 30.f));
            m_boundsCeil = m_boundsFloor + height;
        }
        m_hasTappedThisGamemode = false;
        m_lastGamemodePortalPos = player->m_lastPortalPos;

        if (gamemode & PoolState::TAP_FLYING && isClicking(player))
            placeJumpIndicator(playerPos, state & PoolState::GRAVITY_REVERSE, true);
    }

    // place object on beat (spb = seconds per beat)
    float spb = 60.f / getSettings()->getBpm();
    bool onBeat = (int)(m_elapsedTime / spb) != (int)((m_elapsedTime + dt) / spb);
    bool onHalfBeat = (int)(m_elapsedTime / spb * 2.f) != (int)((m_elapsedTime + dt) / spb * 2.f);

    bool shouldPlace = false;
    int excludeTags = 0;
    int requireTap = 0;
    if (m_placeAgainTimer >= 0)
        m_placeAgainTimer--;
    if (!useRandomClicks)
    {
        if (isClicking(player) && !player->m_isDashing)
        {
            requireTap |= PoolTap::TAP | PoolTap::HOLD;
            if (state & PoolState::NOT_FLYING)
            {
                // tap is buffered
                if (player->m_stateRingJump)
                    shouldPlace = true;
                else
                    excludeTags |= PoolTag::RING_BUFFERED;
            }
            else
            {
                if (!m_isClickingLastFrame)
                    shouldPlace = true;
                else
                    excludeTags |= PoolTag::RING;
            }
        }
        else
        {
            requireTap |= PoolTap::NO | PoolTap::ANY;
        }
    }

    if (shouldPlace)
        ;
    else if (m_placeAgainTimer == 0)
    {
        shouldPlace = true;
        if (useRandomClicks && m_lastPlacedFish->tags & PoolTag::SPIDER)
            requireTap |= PoolTap::NO | PoolTap::ANY;
    }
    // avoid reaching terminal velocity (-15.f)
    else if (playerVel.y * sign < -10.f && onHalfBeat)
    {
        shouldPlace = true;
        excludeTags |= PoolTag::FALL;
    }
    // beat
    else if (utils::random::chance(15.0 / 16.0) && onBeat)
    {
        shouldPlace = true;
        if (useRandomClicks && utils::random::chance(0.5))
            requireTap |= PoolTap::TAP_OR_HOLD;
    }
    // half beat
    else if (utils::random::chance(0.5) && onHalfBeat)
        shouldPlace = true;
    // flying fallback
    else if (gamemode & PoolState::FLYING && onHalfBeat)
        shouldPlace = true;

    // prevent placing two objects in two sequential frames
    if (!m_canPlaceNextFrame)
        shouldPlace = false;

    // continue placing a block platform
    if (m_lastPlacedFish && m_lastPlacedFish->keepActive)
        placeFish(m_lastPlacedFish, !shouldPlace, true);

    // place new object
    const PoolObject *fish;
    if (shouldPlace)
    {
        // only change gamemode, speed, and size on beat
        if (!onBeat)
            excludeTags |= PoolTag::GAMEMODE | PoolTag::SPEED | PoolTag::SIZE_;

        fish = fishLegally(excludeTags, requireTap);
        if (fish)
        {
            placeFish(fish);

            // adjust tap balance
            if (fish->tap != PoolTap::ANY)
            {
                m_shouldTap = fish->tap;
            }
            if (fish->tap == PoolTap::NO)
            {
                m_tapBalance -= 2.f;
            }
            else if (fish->tap == PoolTap::TAP)
            {
                m_tapBalance += 1.f;
            }
            else if (fish->tap == PoolTap::TAP_DELAYED)
            {
                m_tapBalance += 1.f;
                m_shouldTap = PoolTap::TAP;
                m_shouldTapTimer = 3;
            }
            else if (fish->tap == PoolTap::HOLD || fish->tap == PoolTap::HOLD_RANDOM)
            {
                m_tapBalance += .5f;
            }
            else if (fish->tap == PoolTap::RANDOM)
            {
                m_tapBalance += .5f;
            }
            else if (fish->tap == PoolTap::ANY)
            {
                m_tapBalance -= 1.f;
                if (gamemode & PoolState::NOT_FLYING)
                    m_shouldTap = PoolTap::NO;
                else if (gamemode & PoolState::HOLD_FLYING && utils::random::chance(0.5))
                    m_shouldTap = utils::random::chance(0.5) ? PoolTap::NO : PoolTap::HOLD;
                // place another object soon, because we can
                // maybe a fish->canPlaceAgain field would be useful in the future? or fish->placeAgainChance?
                if (utils::random::chance(0.5))
                {
                    m_placeAgainTimer = 2;
                    // ensure we have enough ground for the next fish if the player is grounded currently
                    if (state & PoolState::GROUNDED)
                    {
                        auto fish2 = PoolObject("!! GROUNDED PLACEAGAIN !!")
                                         .withObjectId(ObjectId::BLOCK)
                                         .withAlign(PoolAlign::BC, PoolAlign::TC);
                        placeFish(&fish2);
                    }
                }
            }

            // place a no-tap object after a spider tap
            if (fish->tags & PoolTag::SPIDER && utils::random::chance(0.5))
            {
                m_placeAgainTimer = 2;
            }

            // make sure the player lets go of jump for at least 1 frame before tapping a ring or robot jump
            if (useRandomClicks && fish->tap & PoolTap::TAP_OR_HOLD && isClicking(player))
            {
                if (fish->tags & PoolTag::RING || (state & PoolState::JUMP_NEEDS_BUFFER && fish->tags & PoolTag::BLOCK) || player->m_isDashing)
                {
                    player->releaseButton(PlayerButton::Jump);
                    m_shouldTapTimer = std::max(1, m_shouldTapTimer);
                }
            }

            m_canPlaceNextFrame = false;
        }
        else
        {
            log::warn("fishing failed! last id: {}", m_fishId);
            m_canPlaceNextFrame = true;
        }
    }
    else
    {
        m_canPlaceNextFrame = true;
    }

    // spikes v2
    // const float evilSpikeMargin = trail.playerSize * 15.f + 7.f;
    const float scanBehindPlayer = 60.f;
    float yMin = FLT_MAX;
    float yMax = -FLT_MAX;
    PlayerTrailData leftTrail;
    PlayerTrailData midTrail;
    float spikeX = playerPos.x - scanBehindPlayer / 2;
    for (auto it = m_playerTrail.rbegin(); it != m_playerTrail.rend(); ++it)
    {
        auto trail = *it;
        leftTrail = trail;
        if (trail.pos.x > spikeX)
            midTrail = trail;
        if (trail.pos.x < playerPos.x - scanBehindPlayer)
            break;
        float spikeMargin;
        if (trail.state & PoolState::GAMEMODE_WAVE)
            spikeMargin = 45.f + (trail.state & PoolState::SIZE_MINI ? 10.f : 0.f);
        else if (trail.state & PoolState::GAMEMODE_SHIP)
            spikeMargin = 65.f - (trail.state & PoolState::SIZE_MINI ? 10.f : 0.f);
        else
            spikeMargin = 70.f;
        yMin = std::min(yMin, trail.pos.y - spikeMargin);
        yMax = std::max(yMax, trail.pos.y + spikeMargin);
    }
    auto playerRect = player->getObjectRect();
    placeSpikeBoundary(
        CCPoint{spikeX, yMin},
        CCPoint{spikeX, yMax},
        leftTrail.pos,
        midTrail,
        playerPos,
        (leftTrail.state & PoolState::GAMEMODE_SPIDER || state & PoolState::GAMEMODE_SPIDER
             // when performing a spider teleport, the player uses the inner rect for collision,
             // which is approximately the same width as a spike (6). except for wave, which is not handled.
             // it does not change in mini size.
             ? 6.f
             : playerRect.size.width) +
            // add one spike width minus xv
            6.f - playerVel.x);

    // jumping
    if (useRandomClicks)
    {
        if (m_shouldTapTimer > 0)
        {
            m_shouldTapTimer--;
        }
        else if (m_shouldTap == PoolTap::RANDOM)
        {
            // 50% chance to toggle the jump button, every couple of frames
            if (utils::random::chance(0.5))
            {
                m_shouldTapTimer = utils::random::generate<int>(2, 5);
                if (isClicking(player))
                    player->releaseButton(PlayerButton::Jump);
                else
                    player->pushButton(PlayerButton::Jump);
            }
        }
        else if ((bool)(m_shouldTap & PoolTap::TAP_OR_HOLD) != isClicking(player))
        {
            if (m_shouldTap & PoolTap::TAP_OR_HOLD)
            {
                player->pushButton(PlayerButton::Jump);
                m_hasTappedThisGamemode = true;

                // release the button if required
                if (m_shouldTap & (PoolTap::TAP | PoolTap::HOLD_RANDOM))
                {
                    // delay a couple frames to simulate realistic clicking
                    // however, ufo can 'hold to fly' when using player->pushButton, so don't do it then
                    if (!(state & PoolState::TAP_FLYING))
                    {
                        // hold for a random duration
                        if (m_shouldTap == PoolTap::HOLD_RANDOM)
                            m_shouldTapTimer = utils::random::generate<int>(2, 21);
                        else
                            m_shouldTapTimer = 3;
                    }
                    m_shouldTap = PoolTap::NO;
                }
            }
            else
            {
                player->releaseButton(PlayerButton::Jump);
            }
        }
    }
    else
    {
        if (isClicking(player))
            m_hasTappedThisGamemode = true;
    }

    // make debug trail
    if (getSettings()->getMakeDebugTrail())
    {
        auto trailObj = editor->createObject(
            isClicking(player) ? ObjectId::TRAIL_INDICATOR_CLICKING : ObjectId::TRAIL_INDICATOR,
            playerPos,
            true);
        trailObj->updateCustomScaleX(0.5);
        trailObj->updateCustomScaleY(0.5);
        trailObj->m_editorLayer = 1;

        if (fish)
        {
            auto trailObj2 = editor->createObject(ObjectId::TRAIL_INDICATOR_PLACING, playerPos, true);
            trailObj2->updateCustomScaleX(2.0);
            trailObj2->updateCustomScaleY(0.5);
            trailObj2->setRotation(90.f);
            trailObj2->m_editorLayer = 1;
        }
    }

    // jump indicators
    if (getSettings()->getMakeJumpIndicators())
    {
        if (state & PoolState::NOT_FLYING)
        {
            // consecutive jumps NEVER REGISTER THE PLAYER AS GROUNDED!!!
            // idk what to do here, maybe it's fine as is
            if (m_playerTrail.size() > 1)
            {
                auto trail = m_playerTrail[m_playerTrail.size() - 2];
                // absolutely miserable workaround
                if (isClicking(player) && (m_isClickingLastFrame || !useRandomClicks) && trail.state & PoolState::GROUNDED && trail.pos.y != playerPos.y)
                    placeJumpIndicator(trail.pos, trail.state & PoolState::GRAVITY_REVERSE, false);
            }
        }
        else if (state & PoolState::TAP_FLYING)
        {
            if (isClicking(player) && !m_isClickingLastFrame && (fish == nullptr || !(fish->tags & PoolTag::RING)))
                placeJumpIndicator(playerPos, state & PoolState::GRAVITY_REVERSE, true);
        }
    }

    m_elapsedTime += dt;
    m_isClickingLastFrame = isClicking(player);
    m_lastPlayerGamemode = gamemode;
}

const PoolObject *LayoutGeneratorLayer::fishLegally(int excludeTags, int requireTap)
{
    MACRO_PLAYER_DATA;

    // playerFollowFloats usage
    // auto followFloats = player->m_playerFollowFloats;
    // auto followIndex = player->m_followRelated + followFloats.size();
    // auto followAWhileAgo = followFloats[(followIndex - 18) % followFloats.size()];

    bool isBlind = false;
    if (m_playerTrail.size() > 18)
        isBlind = abs(playerPos.y - m_playerTrail[m_playerTrail.size() - 18].pos.y) > 130.f;

    return GameObjectPool::fish(
        [&](const PoolObject *fish)
        {
            // tag blacklist
            if (fish->tags & excludeTags || fish->tags & getSettings()->getExcludeTags())
                return 0.f;

            // require tap
            if (requireTap > 0 && !(fish->tap & requireTap))
                return 0.f;

            // match state
            if (!fish->matchesPlayerState(state))
                return 0.f;

            // !! important things to never do !!

            // late rings in cube, ball, robot, spider
            if (state & PoolState::NOT_FLYING && fish->tags & PoolTag::RING_LATE)
                return 0.f;

            // blind jumps
            if (isBlind && fish->tap & PoolTap::TAP_OR_HOLD)
                return 0.f;

            // chaining a portal into another portal just looks bad
            if (m_placeAgainTimer == 0 && fish->tap == PoolTap::ANY)
                return 0.f;

            // tapping a green orb that results in the player dying to the floor boundary
            if (state & PoolState::NO_BOUNDS && !upsideDown && playerPos.y < m_boundsFloor + 135.f && fish->tags & PoolTag::FALL && fish->tags && PoolTag::GRAVITY)
                return 0.f;

            // changing gamemode when it hasn't tapped yet
            if (!m_hasTappedThisGamemode && fish->tags & PoolTag::GAMEMODE)
                return 0.f;

            // tap balance
            float weight = 1.f;
            if (m_tapBalance < 0 && fish->tap & (PoolTap::NO | PoolTap::ANY))
                weight = 1.f / -m_tapBalance;
            if (m_tapBalance > 0 && fish->tap & (PoolTap::TAP_OR_HOLD | PoolTap::RANDOM))
                weight = 1.f / m_tapBalance;

            if (state & PoolState::NO_BOUNDS)
            {
                // hitting the bounds kills you
                if (upsideDown)
                {
                    if (playerPos.y < m_boundsFloor + 225.f && fish->tags & PoolTag::JUMP_HIGH)
                        return 0.f;
                    if (playerPos.y < m_boundsFloor + 150.f && fish->tags & PoolTag::JUMP)
                        return 0.f;
                }
                else
                {
                    if (playerPos.y > m_boundsCeil - 210.f && fish->tags & PoolTag::JUMP_HIGH)
                        return 0.f;
                    if (playerPos.y > m_boundsCeil - 135.f && fish->tags & PoolTag::JUMP)
                        return 0.f;
                }

                // if cube is too high, correct it immediately
                if (playerPos.y > m_boundsCeil - 210.f)
                {
                    if (upsideDown && fish->tags & PoolTag::FALL)
                        return 0.f;
                    if (!upsideDown && fish->tags & PoolTag::JUMP)
                        return 0.f;
                    if (upsideDown && fish->tags & PoolTag::GRAVITY)
                        weight *= 10.f;
                }
            }
            // aim for middle of bounds
            else
            {
                auto mid = (m_boundsFloor + m_boundsCeil) / 2.f;
                auto dist = abs(playerPos.y - mid);
                if (playerPos.y * sign > mid * sign)
                {
                    if (fish->tags & PoolTag::JUMP)
                        weight *= 1 - dist / 125.f;
                }
                else
                {
                    if (fish->tags & PoolTag::FALL)
                        weight *= 1 - dist / 125.f;
                }
                // dividing by 125 is arbitrary, but somewhat based on the maximum distance of 135
            }

            return weight;
        });
}

void LayoutGeneratorLayer::placeFish(const PoolObject *fish, bool dedup, bool useLastY)
{
    log::info("{} {}", m_fishId, fish->name);

    MACRO_PLAYER_DATA;
    auto playerRect = player->getObjectRect();

    // positioning and placing
    auto pos = CCPoint{
        playerPos.x + playerVel.x,
        playerPos.y + (state & PoolState::GROUNDED ? 0.f : playerVel.y)};
    if (fish->alignPlayer & PoolAlign::T)
        pos.y += playerRect.size.height / 2.f * sign;
    else if (fish->alignPlayer & PoolAlign::B)
        pos.y -= playerRect.size.height / 2.f * sign;
    if (fish->alignPlayer & PoolAlign::L)
        pos.x -= playerRect.size.width / 2.f;
    else if (fish->alignPlayer & PoolAlign::R)
        pos.x += playerRect.size.width / 2.f;

    GameObject *primaryObj = nullptr;
    if (fish->objectId >= 0)
    {
        auto tempObj = editor->createObject(fish->objectId, CCPoint{}, true);
        if (upsideDown)
        {
            if (fish->canFlip())
                tempObj->setFlipY(true);
            tempObj->setRotation(-fish->rotation);
        }
        else
            tempObj->setRotation(fish->rotation);

        auto primaryObjRect = getObjectRect(tempObj);
        editor->removeObject(tempObj, true);
        if (fish->alignObject & PoolAlign::T)
            pos.y -= primaryObjRect.getMaxY() * sign;
        else if (fish->alignObject & PoolAlign::B)
            pos.y -= primaryObjRect.getMinY() * sign;
        if (fish->alignObject & PoolAlign::L)
            pos.x -= primaryObjRect.getMinX();
        else if (fish->alignObject & PoolAlign::R)
            pos.x -= primaryObjRect.getMaxX();
        if (useLastY)
            pos.y = m_lastPlacedFishPos.y;

        // check if ok to place the object at the new position
        bool ok = true;
        if (isOutOfBounds(pos.y, primaryObjRect.size.height, state & PoolState::HAS_BOUNDS))
            ok = false;
        else if (dedup && getObjectNearPoint(pos, 24.f, fish->objectId) != nullptr)
            ok = false;

        // check if the object interferes with the last couple frames
        if (ok && !(fish->tags & PoolTag::RING))
        {
            primaryObjRect.origin += pos;
            if (doesRectInterfereWithTrail(primaryObjRect, playerPos.x, fish->tags & PoolTag::BLOCK, state & PoolState::SIZE_MINI))
            {
                ok = false;
                log::warn("{} {} CANCELLED due to trail interference!", m_fishId, fish->name);
            }
        }

        if (ok)
        {
            primaryObj = editor->createObject(fish->objectId, pos, true);
            if (upsideDown)
            {
                if (fish->canFlip())
                    primaryObj->setFlipY(true);
                primaryObj->setRotation(-fish->rotation);
            }
            else
                primaryObj->setRotation(fish->rotation);

            primaryObjRect = getObjectRect(primaryObj);

            // spider pad and ring patch
            if (fish->tags & PoolTag::SPIDER)
            {
                if (fish->rotation == 180.f)
                {
                    primaryObj->setFlipY(!primaryObj->m_isFlipY);
                    primaryObj->setRotation(0.f);
                }
                primaryObj->customSetup();
            }

            // every ring patch
            if (auto ringObj = typeinfo_cast<RingObject *>(primaryObj))
            {
                if (primaryObjRect.intersectsRect(playerRect))
                {
                    // yep this is all necessary
                    player->addToTouchedRings(ringObj);
                    if (getSettings()->getUsePlayerClicks() && state & PoolState::FLYING && fish->tags & PoolTag::RING_LATE)
                    {
                        if (state & PoolState::GAMEMODE_SWING)
                            player->flipGravity(!player->m_isUpsideDown, true);
                        player->pushButton(PlayerButton::Jump);
                    }
                }
            }

            // place d blocks in wave
            if (fish->objectId == ObjectId::GAMEMODE_PORTAL_WAVE || (state & PoolState::GAMEMODE_WAVE && fish->objectId == ObjectId::BLOCK))
                placeDBlock(pos);
        }
    }

    // place ground below spider taps, pads, and rings
    if (fish->tags & PoolTag::SPIDER && (getSettings()->getUseRandomClicks() || !(fish->tags & PoolTag::BLOCK)))
    {
        bool up = upsideDown != (bool)(fish->tags & PoolTag::GRAVITY);
        float yMin, yMax;
        if (up)
        {
            yMin = playerPos.y + (fish->tags & PoolTag::GRAVITY ? 30.f : 60.f);
            yMax = state & PoolState::HAS_BOUNDS ? m_boundsCeil : yMin + 150.f;
        }
        else
        {
            yMax = playerPos.y - (fish->tags & PoolTag::GRAVITY ? 30.f : 60.f);
            yMin = state & PoolState::HAS_BOUNDS ? m_boundsFloor : yMax - 150.f;
        }
        float y = utils::random::generate<float>(yMin, yMax);
        bool didPlace;
        while (true)
        {
            auto obj = editor->createObject(ObjectId::BLOCK, CCPoint{playerPos.x + playerVel.x, y}, true);
            if (!doesRectInterfereWithTrail(getObjectRect(obj), playerPos.x, true, state & PoolState::SIZE_MINI))
            {
                didPlace = true;
                break;
            }
            editor->removeObject(obj, true);
            if (state & PoolState::HAS_BOUNDS)
            {
                didPlace = false;
                break;
            }
            y += 30.f * (up ? 1 : -1);
        }
        if (didPlace && state & PoolState::GAMEMODE_WAVE)
            placeDBlock(CCPoint{playerPos.x + playerVel.x, y});
    }

    // place label
    if (getSettings()->getMakeDebugTrail() && !useLastY)
    {
        placeLabel(
            fish->name,
            CCPoint{pos.x, pos.y - 60.f});
        placeLabel(
            std::to_string(m_fishId),
            CCPoint{pos.x, pos.y - 67.5f});
    }

    // store this fish
    m_fishId++;
    m_lastPlacedFish = fish;
    m_lastPlacedFishPos = pos;
}

void LayoutGeneratorLayer::placeCreditText(std::string text, CCPoint pos)
{
    auto textObj = static_cast<TextGameObject *>(LevelEditorLayer::get()->createObject(ObjectId::TEXT, pos, true));
    textObj->updateCustomScaleX(0.5);
    textObj->updateCustomScaleY(0.5);
    textObj->m_zLayer = ZLayer::T2;
    textObj->updateTextObject(text, false);
}

void LayoutGeneratorLayer::placeDBlock(CCPoint pos)
{
    auto dBlockObj = LevelEditorLayer::get()->createObject(ObjectId::D_BLOCK, pos, true);
    dBlockObj->updateCustomScaleX(3.0);
    dBlockObj->updateCustomScaleY(3.0);
}

void LayoutGeneratorLayer::placeJumpIndicator(CCPoint pos, bool isUpsideDown, bool isFlying)
{
    int sign = isUpsideDown ? -1 : 1;
    auto obj = LevelEditorLayer::get()->createObject(
        isFlying ? ObjectId::JUMP_INDICATOR_FLYING : ObjectId::JUMP_INDICATOR_GROUNDED,
        pos + CCPoint{0.f, isFlying ? 0.f : -5.f * sign},
        true);
    if (isFlying)
    {
        obj->updateCustomScaleX(0.5);
        obj->updateCustomScaleY(0.5);
    }
    else
    {
        obj->setRotation(90.f * sign);
    }
    obj->m_editorLayer = 1;
}

void LayoutGeneratorLayer::placeLabel(std::string text, CCPoint pos)
{
    auto textObj = static_cast<TextGameObject *>(LevelEditorLayer::get()->createObject(ObjectId::TEXT, pos, true));
    textObj->updateCustomScaleX(0.25);
    textObj->updateCustomScaleY(0.25);
    textObj->m_editorLayer = 1;
    textObj->m_zLayer = ZLayer::T2;
    textObj->updateTextObject(text, false);
}

void LayoutGeneratorLayer::placeSpikeBoundary(
    CCPoint spikeBottomPos,
    CCPoint spikeTopPos,
    CCPoint leftPos,
    const PlayerTrailData &midTrail,
    CCPoint rightPos,
    float dedupDistance)
{
    auto midPos = midTrail.pos;
    auto midState = midTrail.state;

    // wave (slopes) (unused)
    // if (midState & PoolState::GAMEMODE_WAVE)
    // {
    //     int objectId = midState & PoolState::SIZE_MINI ? ObjectId::SLOPE_GENTLE : ObjectId::SLOPE;
    //     if (
    //         !isOutOfBounds(spikeBottomPos.y, 30.f, hasBounds) && getObjectNearPoint(spikeBottomPos, 30.f, ObjectId::SLOPE) == nullptr && getObjectNearPoint(spikeBottomPos, 30.f, ObjectId::SLOPE_GENTLE) == nullptr)
    //     {
    //         auto bottomSpike = editor->createObject(objectId, spikeBottomPos, true);
    //         if (rightPos.y < midPos.y)
    //             bottomSpike->setRotation(90.f);
    //         else
    //         {
    //             bottomSpike->setFlipX(true);
    //             bottomSpike->setRotation(-90.f);
    //         }
    //     }
    //     if (!isOutOfBounds(spikeTopPos.y, 30.f, hasBounds) && getObjectNearPoint(spikeTopPos, 30.f, ObjectId::SLOPE) == nullptr && getObjectNearPoint(spikeTopPos, 30.f, ObjectId::SLOPE_GENTLE) == nullptr)
    //     {
    //         auto topSpike = editor->createObject(objectId, spikeTopPos, true);
    //         if (rightPos.y > midPos.y)
    //         {
    //             topSpike->setFlipY(true);
    //             topSpike->setRotation(-90.f);
    //         }
    //         else
    //             topSpike->setRotation(-90.f);
    //     }
    // }

    if (leftPos.y < midPos.y - 1.f && rightPos.y < midPos.y - 1.f && midState & PoolState::GRAVITY_NORMAL)
        spikeBottomPos.y += midState & PoolState::SIZE_MINI ? 30.f : 15.f;
    if (leftPos.y > midPos.y + 1.f && rightPos.y > midPos.y + 1.f && midState & PoolState::GRAVITY_REVERSE)
        spikeTopPos.y -= midState & PoolState::SIZE_MINI ? 30.f : 15.f;

    // bottom
    if (getObjectNearPoint(spikeBottomPos, dedupDistance, ObjectId::SPIKE) == nullptr)
    {
        placeSpikeInBounds(spikeBottomPos, midTrail, false);
        if (m_lastSpikeBottomPos.x > 0)
        {
            CCPoint spikeBottomPos2 = spikeBottomPos;
            while (spikeBottomPos2.y > m_lastSpikeBottomPos.y + 30.f)
            {
                spikeBottomPos2.y -= 30.f;
                placeSpikeInBounds(spikeBottomPos2, midTrail, false);
            }
            while (spikeBottomPos.y < m_lastSpikeBottomPos.y - 30.f)
            {
                m_lastSpikeBottomPos.y -= 30.f;
                placeSpikeInBounds(m_lastSpikeBottomPos, midTrail, false);
            }
        }
        m_lastSpikeBottomPos = spikeBottomPos;
    }

    // top
    if (getObjectNearPoint(spikeTopPos, dedupDistance, ObjectId::SPIKE) == nullptr)
    {
        placeSpikeInBounds(spikeTopPos, midTrail, true);
        if (m_lastSpikeTopPos.x > 0)
        {
            while (spikeTopPos.y > m_lastSpikeTopPos.y + 30.f)
            {
                m_lastSpikeTopPos.y += 30.f;
                placeSpikeInBounds(m_lastSpikeTopPos, midTrail, true);
            }
            CCPoint spikeTopPos2 = spikeTopPos;
            while (spikeTopPos2.y < m_lastSpikeTopPos.y - 30.f)
            {
                spikeTopPos2.y += 30.f;
                placeSpikeInBounds(spikeTopPos2, midTrail, true);
            }
        }
        m_lastSpikeTopPos = spikeTopPos;
    }
}

void LayoutGeneratorLayer::placeSpikeInBounds(CCPoint pos, const PlayerTrailData &trail, bool flipY)
{
    if (!isOutOfBounds(pos.y, 12.f, trail.state & PoolState::HAS_BOUNDS, trail.boundsCeil, trail.boundsFloor))
    {
        LevelEditorLayer::get()->createObject(ObjectId::SPIKE, pos, true)->setFlipY(flipY);
    }
}

bool LayoutGeneratorLayer::doesRectInterfereWithTrail(CCRect primaryObjRect, float playerX, bool isBlock, bool isMini)
{
    for (auto it = m_playerTrail.rbegin(); it != m_playerTrail.rend(); ++it)
    {
        auto trail = *it;
        if (trail.pos.x > playerX + 10.f)
            continue;
        if (trail.pos.x < playerX - 30.f)
            break;

        // construct the player rect
        auto rect = CCRect(
            trail.pos.x - trail.rectWidth / 2.f,
            trail.pos.y - trail.rectWidth / 2.f,
            trail.rectWidth,
            trail.rectWidth);

        // shrink slightly if it's a block because of the inner hitbox tolerance
        if (isBlock)
            rect.inflateRect(isMini ? -2.f : -5.f);

        // check intersection
        if (rect.intersectsRect(primaryObjRect))
            return true;
    }

    return false;
}

bool LayoutGeneratorLayer::isClicking(PlayerObject *player)
{
    return player->m_holdingButtons[(int)PlayerButton::Jump];
}

bool LayoutGeneratorLayer::isOutOfBounds(float y, float height, bool hasUpperBound, float boundsCeil, float boundsFloor)
{
    return y + height / 2.f < boundsFloor || (y - height / 2.f > boundsCeil && hasUpperBound);
}

bool LayoutGeneratorLayer::isOutOfBounds(float y, float height, bool hasUpperBound)
{
    return isOutOfBounds(y, height, hasUpperBound, m_boundsCeil, m_boundsFloor);
}

GameObject *LayoutGeneratorLayer::getObjectNearPoint(CCPoint point, float radius, int objectId)
{
    auto editor = LevelEditorLayer::get();

    // if the point might be offscreen, iterate through all of the objects.
    // checking by section only works for things that are onscreen.
    // the editor camera stops roughly around y = 1300.
    bool mayBeOffscreen = point.y > 1300.f;

    if (mayBeOffscreen)
    {
        for (int i = 0; i < editor->m_objects->count(); i++)
        {
            GameObject *obj = static_cast<GameObject *>(editor->m_objects->objectAtIndex(i));
            if (obj == nullptr)
                continue;
            if (objectId >= 0 && obj->m_objectID != objectId)
                continue;
            auto pos = obj->getPosition();
            if (pow(pos.x - point.x, 2) + pow(pos.y - point.y, 2) <= pow(radius, 2))
                return obj;
        }
    }
    else
    {

        int xSectionCenter = std::max(0, (int)(point.x / 100));
        int ySectionCenter = std::max(0, (int)(point.y / 100));

        for (int xSection = xSectionCenter - 1; xSection <= xSectionCenter + 1; xSection++)
        {
            for (int ySection = ySectionCenter - 1; ySection <= ySectionCenter + 1; ySection++)
            {
                if (xSection > editor->m_sections.size() - 1 || editor->m_sections[xSection] == nullptr)
                    continue;

                auto &col = *editor->m_sections[xSection];
                if (ySection > col.size() - 1 || col[ySection] == nullptr)
                    continue;

                for (auto &obj : *col[ySection])
                {
                    if (obj == nullptr)
                        continue;
                    // WHY IS THIS A FIELD ROBTOP
                    if (!obj->m_isActivated)
                        continue;
                    if (objectId >= 0 && obj->m_objectID != objectId)
                        continue;
                    auto pos = obj->getPosition();
                    if (pow(pos.x - point.x, 2) + pow(pos.y - point.y, 2) <= pow(radius, 2))
                        return obj;
                }
            }
        }
    }

    return nullptr;
}

// https://github.com/TheSillyDoggo/GeodeMenu/blob/c2452e1242d063be21188afc81c0cd2c7c47224c/src/Hacks/Level/Hitboxes/HitboxNode.cpp#L140
CCRect LayoutGeneratorLayer::getObjectRect(GameObject *obj)
{
    if (obj->m_isObjectRectDirty)
    {
        auto save1 = obj->m_isObjectRectDirty;
        auto save2 = obj->m_boxOffsetCalculated;

        auto rect = obj->getObjectRect();

        obj->m_isObjectRectDirty = save1;
        obj->m_boxOffsetCalculated = save2;

        return rect;
    }
    else
    {
        return obj->m_objectRect;
    }
}

PoolState LayoutGeneratorLayer::getPlayerGamemode(PlayerObject *player)
{
    if (player->m_isShip)
        return PoolState::GAMEMODE_SHIP;
    else if (player->m_isBall)
        return PoolState::GAMEMODE_BALL;
    else if (player->m_isBird)
        return PoolState::GAMEMODE_UFO;
    else if (player->m_isDart)
        return PoolState::GAMEMODE_WAVE;
    else if (player->m_isRobot)
        return PoolState::GAMEMODE_ROBOT;
    else if (player->m_isSpider)
        return PoolState::GAMEMODE_SPIDER;
    else if (player->m_isSwing)
        return PoolState::GAMEMODE_SWING;
    else
        return PoolState::GAMEMODE_CUBE;
}

int LayoutGeneratorLayer::getPlayerState(PlayerObject *player)
{
    int state = 0;

    auto yv = player->getYVelocity() * (player->m_isUpsideDown ? -1 : 1);
    if (yv > 1.f)
        state |= PoolState::RISING;
    else if (yv < -1.f)
        state |= PoolState::FALLING;
    if (abs(yv) < 3)
        state |= PoolState::PEAKING;
    state |= player->m_isUpsideDown ? PoolState::GRAVITY_REVERSE : PoolState::GRAVITY_NORMAL;
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
    state |= getPlayerGamemode(player);

    // ground detection (doesn't work for flying gamemodes)
    if (state & PoolState::NOT_FLYING)
        state |= player->m_isOnGround ? PoolState::GROUNDED : PoolState::AIRBORNE;
    // log::debug("{} {} {} {} {}", player->m_isOnGround, player->m_isOnGround2, player->m_isOnGround3,
    // player->m_isOnGround4, yv);

    return state;
}

void LayoutGeneratorLayer::onBuildButton(CCObject *)
{
    auto editor = LevelEditorLayer::get();

    // stop building
    if (m_isBuilding)
    {
        editor->m_editorUI->onStopPlaytest(nullptr);
    }
    // start building
    else
    {
        // for parsing guides:
        // use editor->m_levelSettings->m_guidelineString, split on ~0~
        reset();
        placeCreditText("made with", CCPoint{105.f, 300.f});
        placeCreditText("layout generator", CCPoint{105.f, 285.f});
        editor->m_editorUI->onPlaytest(nullptr);
    }
}

void LayoutGeneratorLayer::onSettingsButton(CCObject *)
{
    // auto editor = LevelEditorLayer::get();

    // auto x = editor->m_sections;
    // for (int i = 0; i < x.size(); i++)
    // {
    //     if (x[i] == nullptr)
    //         continue;
    //     auto &y = *x[i];
    //     for (int j = 0; j < y.size(); j++)
    //     {
    //         if (y[j] == nullptr)
    //             continue;
    //         auto &z = *y[j];
    //         for (int k = 0; k < z.size(); k++)
    //         {
    //             GameObject *obj = z[k];
    //             if (obj == nullptr || !obj->m_isActivated)
    //                 continue;
    //             if (obj->m_editorLayer != 0)
    //                 continue;
    //             auto pos = obj->getPosition();
    //             // log::debug("ID {} AT {} {} INDEX {} {} {} DEBUG {} {} {} {} {} {} {} {}",
    //             // obj->m_objectID, pos.x, pos.y, i, j, k, obj->m_editorEnabled, obj->m_isActivated,
    //             // obj->m_isDirty, obj->m_isDisabled, obj->m_isDisabled2, obj->m_isUnmodifiedPosDirty,
    //             // obj->m_isInvisible, obj->m_isInvisibleBlock);
    //             placeLabel(std::format("{},{}", i, j), CCPoint{pos.x, pos.y + 7.5f});
    //             placeLabel(std::format("{},{}", std::max(0, (int)(pos.x / 100)), std::max(0, (int)(pos.y / 100))), CCPoint{pos.x, pos.y - 7.5f});
    //         }
    //     }
    // }

    auto popup = SettingsPopup::create(getSettings());
    popup->show();
}

void LayoutGeneratorLayer::playtestStopped()
{
    m_isBuilding = false;
}
