#include "LayoutGeneratorLayer.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include "../GameObjectPool/GameObjectPool.hpp"
#include "../PlayerData/PlayerData.hpp"
#include "../PoolObject/PoolObject.hpp"
#include "../PlayerObject.cpp"
#include "../Settings/ObjectSettings.hpp"

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
        return false;

    // for debugging CCRects
    // log::debug("{} {} {} {}", rect.getMinX(), rect.getMinY(), rect.getMaxX(), rect.getMaxY());

    // schedule update
    scheduleUpdate();

    return true;
}

void LayoutGeneratorLayer::reset()
{
    auto mod = Mod::get();

    m_isBuilding = true;
    m_boundsCeil = 1300.f;
    m_boundsFloor = 90.f;
    m_canPlaceNextFrame = true;
    m_elapsedTime = LevelEditorLayer::get()->m_levelSettings->m_songOffset + .133f; // offset for better sync
    // m_fishId = 0; // no need to reset
    m_gamemodePortalY = 0.f;
    m_halfBeatCount = (int)(m_elapsedTime / (60.f / mod->getSettingValue<float>("bpm")) * 2.f);
    m_hasTappedThisGamemode = false;
    m_isClickingLastFrame = false;
    m_lastPlacedFish = nullptr;
    m_lastPlacedFishPos = CCPoint{};
    m_lastPlacedJumpIndicator = nullptr;
    m_lastPlayerGamemode = PoolState::GAMEMODE_CUBE;
    m_lastSpikeBottomPos = CCPoint{};
    m_lastSpikeTopPos = CCPoint{};
    m_placeAgainTimer = -1;
    m_placedObjectUniqueIds.clear();
    m_playerPosPauseCheck = CCPoint{};
    m_playerTrail.clear();
    m_shouldTap = PoolTap::NO;
    m_shouldTapTimer = 0;
    m_tapBalance = 3.f; // don't make the player jump straight away
}

void LayoutGeneratorLayer::buildStart()
{
    // for parsing guides:
    // use editor->m_levelSettings->m_guidelineString, split on ~0~
    reset();
    placeCreditText("made with", CCPoint{105.f, 300.f});
    placeCreditText("layout generator", CCPoint{105.f, 285.f});
    LevelEditorLayer::get()->m_editorUI->onPlaytest(nullptr);
}

void LayoutGeneratorLayer::buildStop()
{
    LevelEditorLayer::get()->m_editorUI->onStopPlaytest(nullptr);
}

void LayoutGeneratorLayer::update(float dt)
{
    CCLayer::update(dt);

    if (!m_isBuilding)
        return;

    // playerdata
    auto mod = Mod::get();
    auto editor = LevelEditorLayer::get();
    auto pd = new PlayerData();
    pd->setPlayer(editor->m_player1);
    if (!pd->player)
    {
        log::error("Player is null, aborting!");
        buildStop();
        return;
    }
    pd->velScaled = CCPoint{pd->velUnscaled.x * 60.f * dt, pd->velUnscaled.y * 60.f * dt};

    // paused
    if (m_playerPosPauseCheck == pd->pos)
        return;
    m_playerPosPauseCheck = pd->pos;

    // trail
    for (auto &trail : pd->player->m_fields->m_queuedTrail)
    {
        if (trail.pos.x > pd->pos.x)
            continue;

        trail.boundsCeil = m_boundsCeil;
        trail.boundsFloor = m_boundsFloor;

        fillSpiderTrail(trail.pos);

        m_playerTrail.push_back(trail);

        // debug trail
        if (mod->getSettingValue<bool>("debug-trail"))
            placeDebugTrailClicking(trail.pos, trail.isClicking);

        // jump indicators v3
        if (mod->getSettingValue<bool>("jump-indicators") && trail.makeJumpIndicator)
            placeJumpIndicator(trail.pos, trail.state);
    }
    pd->player->m_fields->m_queuedTrail.clear();

    fillSpiderTrail(pd->pos);

    // player isn't ready yet
    if (m_playerTrail.empty())
        return;

    // gamemode changed
    if (pd->gamemode != m_lastPlayerGamemode)
    {
        m_gamemodePortalY = pd->player->m_lastPortalPos.y;
        m_hasTappedThisGamemode = false;
    }

    // calculate bounds
    if (pd->gamemode & PoolState::NO_BOUNDS || pd->isCameraFree())
    {
        m_boundsFloor = 90.f;
        m_boundsCeil = 1300.f;
    }
    else
    {
        // GJBaseGameLayer::getGroundHeightForMode
        float height;
        if (pd->gamemode == PoolState::GAMEMODE_BALL)
            height = 240.f;
        else if (pd->gamemode == PoolState::GAMEMODE_SPIDER)
            height = 270.f;
        else // ship and ufo
            height = 300.f;
        // dual height = 270

        float roundMe = m_gamemodePortalY - height / 2.f - 30.f;
        float mid = std::max(90.f, std::ceil(roundMe / 30.f) * 30.f) + height / 2.f;

        // i dont know the official gd method for applying zoom but this seems to work
        float zoom = editor->m_gameState.m_cameraZoom;
        m_boundsFloor = mid - height / 2.f / zoom;
        m_boundsCeil = mid + height / 2.f / zoom;
    }

    // place object on beat (spb = seconds per beat)
    float spb = 60.f / mod->getSettingValue<float>("bpm");
    bool usePlayerClicks = mod->getSettingValue<bool>("use-player-clicks");
    bool useRandomClicks = !usePlayerClicks;
    bool onBeat = false;
    bool onHalfBeat = false;
    if ((int)(m_elapsedTime / spb * 2.f) > m_halfBeatCount)
    {
        onBeat = m_halfBeatCount % 2 == 0;
        onHalfBeat = true;
        m_halfBeatCount++;
    }

    bool shouldPlace = false;
    int excludeTags = 0;
    int requireTap = 0;
    if (m_placeAgainTimer >= 0)
        m_placeAgainTimer--;
    if (usePlayerClicks)
    {
        if (pd->isClicking() && !pd->player->m_isDashing)
        {
            requireTap |= PoolTap::TAP | PoolTap::HOLD;
            if (pd->state & PoolState::NOT_FLYING)
            {
                // tap is buffered
                if (pd->player->m_stateRingJump)
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

            // stop placing block platform
            if (m_lastPlacedFish && m_lastPlacedFish->keepActive)
                m_lastPlacedFish = nullptr;
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
        if (useRandomClicks && m_lastPlacedFish && m_lastPlacedFish->tags & PoolTag::SPIDER)
            requireTap |= PoolTap::NO | PoolTap::ANY;
    }
    // avoid reaching terminal velocity (-15.f)
    else if (pd->velUnscaled.y * pd->getSign() < -10.f &&
             pd->state & (PoolState::NO_BOUNDS | PoolState::GAMEMODE_BALL) && onHalfBeat)
    {
        shouldPlace = true;
        excludeTags |= PoolTag::FALL;
    }
    // beat
    else if (utils::random::chance(15.0 / 16.0) && onBeat)
    {
        shouldPlace = true;
        if (useRandomClicks && !(pd->state & PoolState::HOLD_FLYING) && utils::random::chance(0.5))
            requireTap |= PoolTap::TAP_OR_HOLD;
    }
    // half beat
    else if ((pd->gamemode & PoolState::FLYING || utils::random::chance(3.0 / 4.0)) && onHalfBeat)
    {
        shouldPlace = true;
    }

    // prevent placing two objects in two sequential frames
    if (!m_canPlaceNextFrame)
        shouldPlace = false;

    // continue placing a block platform
    if (m_lastPlacedFish && m_lastPlacedFish->keepActive)
        placeFish(pd, m_lastPlacedFish, !shouldPlace, true);

    // failsafe when jumping into the floor in reverse gravity
    if (pd->isUpsideDown() &&
        (pd->state & PoolState::NO_BOUNDS || pd->isCameraFree()) &&
        pd->state & PoolState::RISING &&
        pd->pos.y < m_boundsFloor + 30.f)
    {
        log::debug("{} FLOOR FAILSAFE", m_fishId);
        CCPoint pos{pd->pos.x, m_boundsFloor + 6.f};
        createObject(ObjectId::SPIDER_PAD, pos)->setFlipY(true);
    }

    // place new object
    const PoolObject *fish = nullptr;
    if (shouldPlace)
    {
        // only change gamemode, speed, and size on beat
        if (!onBeat)
            excludeTags |= PoolTag::GAMEMODE | PoolTag::SPEED | PoolTag::SIZE_;

        // experimental gameplay
        if (!mod->getSettingValue<bool>("experimental-gameplay"))
            excludeTags |= PoolTag::EXPERIMENTAL;

        fish = fishLegally(pd, dt, excludeTags, requireTap);
        if (fish)
        {
            placeFish(pd, fish);

            m_shouldTapTimer = 0;

            // adjust tap balance
            if (fish->tap != PoolTap::ANY)
            {
                m_shouldTap = fish->tap;
            }

            if (fish->tap == PoolTap::NO)
            {
                m_tapBalance -= pd->state & PoolState::HOLD_FLYING ? .5f : 2.f;
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
            else if (fish->tap & (PoolTap::HOLD | PoolTap::HOLD_RANDOM | PoolTap::RANDOM))
            {
                m_tapBalance += .5f;
            }
            else if (fish->tap == PoolTap::ANY)
            {
                m_tapBalance -= 1.f;
                if (pd->gamemode & PoolState::NOT_FLYING)
                    m_shouldTap = PoolTap::NO;
                else if (pd->gamemode & PoolState::HOLD_FLYING && utils::random::chance(0.5))
                    m_shouldTap = pd->isClicking() ? PoolTap::NO : PoolTap::HOLD;

                // place another object soon, because we can
                // maybe a fish->canPlaceAgain field would be useful in the future? or fish->placeAgainChance?
                if (utils::random::chance(0.5))
                {
                    m_placeAgainTimer = 2;

                    // ensure we have enough ground for the next fish if the player is grounded currently
                    if (pd->state & PoolState::GROUNDED)
                    {
                        log::debug("{} {} GROUNDED PLACEAGAIN", m_fishId, fish->name);
                        CCPoint pos = pd->pos;
                        pos.y -= pd->getRectSize().height / 2.f * pd->getSign();
                        pos.y -= 15.f * pd->getSign();
                        if (!isOutOfBounds(pos.y, 30.f, pd))
                            createObject(ObjectId::BLOCK, pos);
                    }
                }
            }
            // PoolTap::TOWARDS_CENTER does not adjust tap balance

            // place a no-tap object after a spider tap
            if (fish->tags & PoolTag::SPIDER && utils::random::chance(0.5))
            {
                m_placeAgainTimer = 2;
            }

            // make sure the player lets go of jump for at least 1 frame before tapping a ring or robot jump
            if (useRandomClicks && fish->tap & PoolTap::TAP_OR_HOLD && pd->isClicking())
            {
                if (fish->tags & PoolTag::RING ||
                    (pd->state & PoolState::JUMP_NEEDS_BUFFER && fish->tags & PoolTag::BLOCK) ||
                    pd->player->m_isDashing)
                {
                    pd->player->releaseButton(PlayerButton::Jump);
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

    m_playerTrail.back().fish = fish;

    // spikes v2
    if (mod->getSettingValue<bool>("spike-boundary-enabled"))
    {
        const float spikeMargin = mod->getSettingValue<float>("spike-margin");
        const CCSize playerSize = pd->getRectSize();

        // slightly larger than a spike hitbox, because just grazing the side of a spike kills you
        // also make it a bit wider according to the spikeMargin
        const CCSize spikeSize{8.f + spikeMargin / 2.f, 14.f};

        // 30 units is the largest width that the player hitbox can ever be
        const float spikeScanRight = pd->pos.x - (30.f - playerSize.width) / 2.f;

        // place the right of the spike on the left of the player
        const float spikeX = spikeScanRight - (playerSize.width + spikeSize.width) / 2.f;

        // scan until the right of the player is past the left of the spike
        const float spikeScanLeft = spikeX - (playerSize.width + spikeSize.width) / 2.f;

        bool spikeBottom = false;
        bool spikeTop = false;
        float yMin = FLT_MAX;
        float yMax = -FLT_MAX;
        PlayerTrailData leftTrail{};
        PlayerTrailData midTrail{};
        bool closeTheSpiderGap = false;
        float midMaxShrink = 0.0;
        for (auto it = m_playerTrail.rbegin(); it != m_playerTrail.rend(); ++it)
        {
            auto trail = *it;

            if (trail.pos.x > spikeScanRight)
                continue;

            // scan range for gravity changes
            if (trail.pos.x < pd->pos.x - pd->velUnscaled.x * 30.f)
                break;

            if (
                // airborne or flying or ball/spider
                trail.state & (PoolState::AIRBORNE | PoolState::HAS_BOUNDS) ||
                // black ring
                (trail.isUpsideDown() && trail.velUnscaled.y >= 15.f) ||
                // spider
                (trail.fish && trail.fish->tags & PoolTag::SPIDER))
                spikeBottom = true;
            if (
                // airborne or flying or ball/spider
                trail.state & (PoolState::AIRBORNE | PoolState::HAS_BOUNDS) ||
                // black ring
                (!trail.isUpsideDown() && trail.velUnscaled.y <= -15.f) ||
                // spider
                (trail.fish && trail.fish->tags & PoolTag::SPIDER))
                spikeTop = true;

            // check for spider elements that would let the player slip through the gaps
            if (trail.pos.x > pd->pos.x - 96.f && trail.pos.x < spikeX)
            {
                if (trail.state & PoolState::GAMEMODE_SPIDER)
                    closeTheSpiderGap = true;
                else if (trail.fish && trail.fish->tags & PoolTag::SPIDER)
                    closeTheSpiderGap = true;
            }

            // scan range for spike positioning
            if (trail.pos.x < spikeScanLeft)
                continue;

            // shrink the bounds a bit more in certain gamemodes where it's too easy
            float shrink = 0.f;
            if (trail.state & PoolState::GAMEMODE_SHIP)
                shrink = 10.f;
            else if (trail.state & PoolState::GAMEMODE_WAVE && trail.state & PoolState::SIZE_NORMAL)
                shrink = 10.f;
            else if (trail.state & PoolState::GAMEMODE_UFO && trail.state & PoolState::SIZE_MINI)
                shrink = 10.f;

            float evilSpikeMargin = (trail.rectSize.height + spikeSize.height) / 2.f;
            float shrunkSpikeMargin = evilSpikeMargin + std::max(spikeMargin - shrink, 0.f);
            leftTrail = trail;
            if (trail.pos.x > spikeX)
            {
                midTrail = trail;
                midMaxShrink = shrunkSpikeMargin - evilSpikeMargin;
            }

            yMin = std::min(yMin, trail.pos.y - shrunkSpikeMargin);
            yMax = std::max(yMax, trail.pos.y + shrunkSpikeMargin);
        }

        if (!(midTrail.state & PoolState::GAMEMODE_WAVE))
        {
            float jumpShrink = std::min(midMaxShrink, midTrail.state & PoolState::SIZE_MINI ? 30.f : 15.f);
            if (leftTrail.pos.y < midTrail.pos.y - 1.f &&
                pd->pos.y < midTrail.pos.y - 1.f &&
                !midTrail.isUpsideDown())
                yMin += jumpShrink;
            if (leftTrail.pos.y > midTrail.pos.y + 1.f &&
                pd->pos.y > midTrail.pos.y + 1.f &&
                midTrail.isUpsideDown())
                yMax -= jumpShrink;
        }

        placeSpikeBoundary(
            spikeBottom,
            CCPoint{spikeX, yMin},
            spikeTop,
            CCPoint{spikeX, yMax},
            midTrail,
            spikeMargin <= 0.f
                // zero spike margin will do this just for fun
                ? 0.f
                // otherwise...
                : (closeTheSpiderGap
                       // when performing a spider teleport, the player uses the inner rect for collision,
                       // which is approximately 6 units wide. (except for wave, which is not handled.)
                       // it does not change when mini.
                       ? 6.f
                       : playerSize.width) +
                      // add one spike width minus xv
                      6.f - pd->velScaled.x);
    }

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
                m_shouldTapTimer = utils::random::generate<int>(2, 6);
                if (pd->isClicking())
                    pd->player->releaseButton(PlayerButton::Jump);
                else
                    pd->player->pushButton(PlayerButton::Jump);
            }
        }
        else if (m_shouldTap == PoolTap::TOWARDS_CENTER)
        {
            // aim for middle of bounds
            float mid = (m_boundsFloor + m_boundsCeil) / 2.f;
            if (pd->state & PoolState::GAMEMODE_SHIP)
                mid -= pd->velScaled.y * 10.f;
            else if (pd->state & PoolState::GAMEMODE_WAVE)
                m_shouldTapTimer = 3;

            bool push = pd->pos.y * pd->getSign() < mid * pd->getSign();

            float dist = abs(pd->pos.y - mid);
            if (dist < 60 && utils::random::chance((1 - dist / 60.f) * .5f))
                push = !push;

            if (push != pd->isClicking())
                if (push)
                    pd->player->pushButton(PlayerButton::Jump);
                else
                    pd->player->releaseButton(PlayerButton::Jump);
        }
        else if ((bool)(m_shouldTap & PoolTap::TAP_OR_HOLD) != pd->isClicking())
        {
            if (m_shouldTap & PoolTap::TAP_OR_HOLD)
            {
                pd->player->pushButton(PlayerButton::Jump);
                m_hasTappedThisGamemode = true;

                // release the button if required
                if (m_shouldTap & (PoolTap::TAP | PoolTap::HOLD_RANDOM))
                {
                    // delay a couple frames to simulate realistic clicking
                    // however, ufo can 'hold to fly' when using player->pushButton, so don't do it then
                    if (!(pd->state & PoolState::TAP_FLYING))
                    {
                        // hold for a random duration
                        if (m_shouldTap == PoolTap::HOLD_RANDOM)
                            m_shouldTapTimer = (int)(utils::random::generate<float>(.03f, .33f) / dt);
                        else
                            m_shouldTapTimer = 3;
                    }
                    m_shouldTap = PoolTap::NO;
                }
            }
            else
            {
                pd->player->releaseButton(PlayerButton::Jump);
            }
        }
    }
    else
    {
        if (pd->isClicking())
            m_hasTappedThisGamemode = true;
    }

    // mark that a fish was placed here
    if (mod->getSettingValue<bool>("debug-trail") && fish)
        placeDebugTrailBar(pd->pos);

    m_elapsedTime += dt;
    m_isClickingLastFrame = pd->isClicking();
    m_lastPlayerGamemode = pd->gamemode;
}

GameObject *LayoutGeneratorLayer::createObject(int key, CCPoint position)
{
    if (auto editor = LevelEditorLayer::get())
    {
        if (auto object = editor->createObject(key, position, true))
        {
            m_placedObjectUniqueIds.insert(object->m_uniqueID);
            return object;
        }
    }

    return nullptr;
}

const PoolObject *LayoutGeneratorLayer::fishLegally(PlayerData *pd, float dt, int excludeTags, int requireTap)
{
    // playerFollowFloats usage
    // auto followFloats = player->m_playerFollowFloats;
    // auto followIndex = player->m_followRelated + followFloats.size();
    // auto followAWhileAgo = followFloats[(followIndex - 18) % followFloats.size()];

    auto mod = Mod::get();
    auto objectWhitelist = mod->getSettingValue<std::unordered_set<int>>("objects");

    const int blindScanBehind = (int)(.3f / dt);
    bool isBlind = false;
    if (!mod->getSettingValue<bool>("use-player-clicks") &&
        m_playerTrail.size() > blindScanBehind &&
        pd->state & PoolState::NO_BOUNDS)
        isBlind = abs(pd->pos.y - m_playerTrail[m_playerTrail.size() - blindScanBehind].pos.y) > 130.f;

    return GameObjectPool::fish(
        [&](const PoolObject *fish)
        {
            // tag blacklist
            if (fish->tags & excludeTags)
                return 0.f;

            // require tap
            if (requireTap > 0 && !(fish->tap & requireTap))
                return 0.f;

            // match state
            if (!fish->matchesPlayerState(pd->state))
                return 0.f;

            // object id whitelist
            if (ObjectSettingsNode::OBJECT_ID_WHITELISTABLE.contains(fish->objectId) &&
                !objectWhitelist.contains(fish->objectId))
                return 0.f;

            // !! important things to never do !!

            // blind jumps
            if (isBlind && fish->tap & PoolTap::TAP_OR_HOLD)
                return 0.f;

            // chaining a portal into another portal just looks bad
            if (m_placeAgainTimer == 0 && fish->tap == PoolTap::ANY)
                return 0.f;

            // tapping a green orb that results in the player dying to the floor boundary
            if ((pd->state & PoolState::NO_BOUNDS || pd->isCameraFree()) &&
                !pd->isUpsideDown() &&
                pd->pos.y < m_boundsFloor + 135.f &&
                fish->tags & PoolTag::FALL &&
                fish->tags & PoolTag::GRAVITY)
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

            if (pd->state & PoolState::NO_BOUNDS || pd->isCameraFree())
            {
                // hitting the bounds kills you
                if (pd->isUpsideDown())
                {
                    if (pd->pos.y < m_boundsFloor + 225.f && fish->tags & PoolTag::JUMP_HIGH)
                        return 0.f;
                    if (pd->pos.y < m_boundsFloor + 150.f && fish->tags & PoolTag::JUMP)
                        return 0.f;
                }
                else
                {
                    if (pd->pos.y > m_boundsCeil - 210.f && fish->tags & PoolTag::JUMP_HIGH)
                        return 0.f;
                    if (pd->pos.y > m_boundsCeil - 135.f && fish->tags & PoolTag::JUMP)
                        return 0.f;
                }

                // if cube is too high, correct it immediately
                if (pd->pos.y > m_boundsCeil - 210.f)
                {
                    if (pd->isUpsideDown() && fish->tags & PoolTag::FALL)
                        return 0.f;
                    if (!pd->isUpsideDown() && fish->tags & PoolTag::JUMP)
                        return 0.f;
                    if (pd->isUpsideDown() && fish->tags & PoolTag::GRAVITY)
                        weight *= 10.f;
                }
            }

            // aim for middle of bounds
            else
            {
                float mid = (m_boundsFloor + m_boundsCeil) / 2.f;
                if (pd->state & PoolState::GAMEMODE_SHIP)
                    mid -= pd->velScaled.y * 10.f;

                float dist = abs(pd->pos.y - mid);
                float penalty = 1 - dist /
                                        ((m_boundsCeil - m_boundsFloor) *
                                         (pd->state & (PoolState::GAMEMODE_SHIP | PoolState::GAMEMODE_UFO) ? .1f : .5f));

                // below middle, relative to gravity
                if (pd->pos.y * pd->getSign() < mid * pd->getSign())
                {
                    if (!(pd->state & PoolState::JUMP_CHANGES_GRAVITY))
                    {
                        if ((fish->tags & PoolTag::FALL && !(fish->tags & PoolTag::GRAVITY)) ||
                            (fish->tags & PoolTag::JUMP && fish->tags & PoolTag::GRAVITY))
                            weight *= penalty;
                    }
                    else if (fish->tags & PoolTag::FALL)
                        weight *= penalty;
                }
                else
                {
                    if (!(pd->state & PoolState::JUMP_CHANGES_GRAVITY))
                    {
                        if ((fish->tags & PoolTag::JUMP && !(fish->tags & PoolTag::GRAVITY)) ||
                            (fish->tags & PoolTag::FALL && fish->tags & PoolTag::GRAVITY))
                            weight *= penalty;
                    }
                    else if (fish->tags & PoolTag::JUMP)
                        weight *= penalty;
                }
            }

            return weight;
        });
}

void LayoutGeneratorLayer::placeFish(PlayerData *pd, const PoolObject *fish, bool dedup, bool useLastY)
{
    // this log MIGHT cause crashes
    // log::info("{} {}", m_fishId, fish->name);

    auto mod = Mod::get();
    auto editor = LevelEditorLayer::get();
    auto playerRect = pd->player->getObjectRect();

    // positioning and placing
    auto pos = CCPoint{
        pd->pos.x + pd->velScaled.x,
        pd->pos.y + (pd->state & PoolState::GROUNDED ? 0.f : pd->velScaled.y)};
    if (fish->alignPlayer & PoolAlign::T)
        pos.y += playerRect.size.height / 2.f * pd->getSign();
    else if (fish->alignPlayer & PoolAlign::B)
        pos.y -= playerRect.size.height / 2.f * pd->getSign();
    if (fish->alignPlayer & PoolAlign::L)
        pos.x -= playerRect.size.width / 2.f;
    else if (fish->alignPlayer & PoolAlign::R)
        pos.x += playerRect.size.width / 2.f;

    bool shouldPlace = true;
    GameObject *primaryObj = nullptr;
    if (fish->objectId >= 0)
    {
        // I TRIED THIS AND IT CRASHED THE GAME BUT NOW IT WORKS I GUESS???
        auto tempObj = GameObject::createWithKey(fish->objectId);
        if (pd->isUpsideDown())
        {
            if (fish->canFlip())
                tempObj->setFlipY(true);
            tempObj->setRotation(-fish->rotation);
        }
        else
            tempObj->setRotation(fish->rotation);

        auto tempObjRect = getObjectRect(tempObj);
        if (fish->alignObject & PoolAlign::T)
            pos.y -= tempObjRect.getMaxY() * pd->getSign();
        else if (fish->alignObject & PoolAlign::B)
            pos.y -= tempObjRect.getMinY() * pd->getSign();
        if (fish->alignObject & PoolAlign::L)
            pos.x -= tempObjRect.getMinX();
        else if (fish->alignObject & PoolAlign::R)
            pos.x -= tempObjRect.getMaxX();
        if (useLastY)
            pos.y = m_lastPlacedFishPos.y;

        // check if ok to place the object at the new position
        if (isOutOfBounds(pos.y, tempObjRect.size.height, pd))
            shouldPlace = false;
        else if (dedup && getObjectNearPoint(pos, 24.f, fish->objectId) != nullptr)
            shouldPlace = false;

        // check if the object interferes with the last couple frames
        if (shouldPlace && !(fish->tags & PoolTag::RING))
        {
            tempObjRect.origin += pos;
            if (doesRectInterfereWithTrail(tempObjRect, pd->pos.x, fish->tags & PoolTag::BLOCK, pd->state & PoolState::SIZE_MINI))
            {
                shouldPlace = false;
                log::debug("{} {} CANCELLED due to trail interference!", m_fishId, fish->name);
            }
        }

        if (shouldPlace)
        {
            primaryObj = createObject(fish->objectId, pos);
            if (pd->isUpsideDown())
            {
                if (fish->canFlip())
                    primaryObj->setFlipY(true);
                primaryObj->setRotation(-fish->rotation);
            }
            else
                primaryObj->setRotation(fish->rotation);

            auto primaryObjRect = getObjectRect(primaryObj);

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
                    pd->player->addToTouchedRings(ringObj);
            }

            // enable free mode for gamemode portals
            if (fish->tags & PoolTag::GAMEMODE && mod->getSettingValue<bool>("camera-free-mode"))
            {
                if (auto effectObj = typeinfo_cast<EffectGameObject *>(primaryObj))
                {
                    effectObj->m_cameraIsFreeMode = true;
                }
            }

            // enable preview for speed portals
            if (fish->tags & PoolTag::SPEED)
            {
                if (auto effectObj = typeinfo_cast<EffectGameObject *>(primaryObj))
                {
                    effectObj->m_shouldPreview = true;
                    editor->tryUpdateSpeedObject(effectObj, false);
                }
            }

            // place d blocks in wave
            if (fish->objectId == ObjectId::GAMEMODE_PORTAL_WAVE ||
                (pd->state & PoolState::GAMEMODE_WAVE && fish->objectId == ObjectId::BLOCK))
                placeDBlock(pos);
        }
    }

    // place ground below spider taps, pads, and rings
    if (fish->tags & PoolTag::SPIDER &&
        // if usePlayerClicks is true, it won't work. the player would already have teleported
        // before it has the chance to place the block.
        (!mod->getSettingValue<bool>("use-player-clicks") || !(fish->tags & PoolTag::BLOCK)) &&
        // ensure the fish was actually placed
        shouldPlace)
    {
        bool up = pd->isUpsideDown() != (bool)(fish->tags & PoolTag::GRAVITY);
        float yMin, yMax;
        if (up)
        {
            yMin = pd->pos.y + (fish->tags & PoolTag::GRAVITY ? 30.f : 60.f);
            yMax = pd->state & PoolState::HAS_BOUNDS && !pd->isCameraFree()
                       ? m_boundsCeil
                       : std::min(m_boundsCeil, yMin + 150.f);
        }
        else
        {
            yMax = pd->pos.y - (fish->tags & PoolTag::GRAVITY ? 30.f : 60.f);
            yMin = pd->state & PoolState::HAS_BOUNDS && !pd->isCameraFree()
                       ? m_boundsFloor
                       : std::max(m_boundsFloor, yMax - 150.f);
        }
        CCPoint pos{pd->pos.x + pd->velScaled.x, utils::random::generate<float>(yMin, yMax)};
        auto tempObj = GameObject::createWithKey(ObjectId::BLOCK);
        bool didPlace;
        while (true)
        {
            auto tempObjRect = getObjectRect(tempObj);
            tempObjRect.origin += pos;
            if (!doesRectInterfereWithTrail(tempObjRect, pd->pos.x, true, pd->state & PoolState::SIZE_MINI))
            {
                createObject(ObjectId::BLOCK, pos)->setFlipY(up);
                didPlace = true;
                break;
            }
            if (pd->state & PoolState::HAS_BOUNDS && !pd->isCameraFree())
            {
                didPlace = false;
                break;
            }
            pos.y += 30.f * (up ? 1 : -1);
        }
        if (didPlace && pd->state & PoolState::GAMEMODE_WAVE)
            placeDBlock(pos);
    }

    // force trigger rings when flying (AFTER spider ground)
    if (mod->getSettingValue<bool>("use-player-clicks") &&
        pd->state & PoolState::FLYING &&
        fish->tags & PoolTag::RING_LATE &&
        shouldPlace)
    {
        if (pd->state & PoolState::GAMEMODE_SWING)
            pd->player->flipGravity(!pd->isUpsideDown(), true);
        pd->player->pushButton(PlayerButton::Jump);
        if (pd->state & PoolState::TAP_FLYING && m_lastPlacedJumpIndicator)
            editor->removeObject(m_lastPlacedJumpIndicator, true);
    }

    // place label
    if (mod->getSettingValue<bool>("debug-trail") && !useLastY)
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
    auto textObj = static_cast<TextGameObject *>(createObject(ObjectId::TEXT, pos));
    textObj->updateCustomScaleX(0.5);
    textObj->updateCustomScaleY(0.5);
    textObj->m_zLayer = ZLayer::T2;
    textObj->updateTextObject(text, false);
}

void LayoutGeneratorLayer::placeDBlock(CCPoint pos)
{
    auto obj = createObject(ObjectId::D_BLOCK, pos);
    obj->updateCustomScaleX(3.0);
    obj->updateCustomScaleY(3.0);
}

void LayoutGeneratorLayer::placeDebugTrailBar(CCPoint pos)
{
    auto obj = createObject(ObjectId::TRAIL_INDICATOR_PLACING, pos);
    obj->updateCustomScaleY(0.25);
    obj->setRotation(90.f);
    obj->m_editorLayer = 1;
}

void LayoutGeneratorLayer::placeDebugTrailClicking(CCPoint pos, bool isClicking)
{
    auto obj = createObject(
        isClicking ? ObjectId::TRAIL_INDICATOR_CLICKING : ObjectId::TRAIL_INDICATOR,
        pos);
    obj->updateCustomScaleX(0.25);
    obj->updateCustomScaleY(0.25);
    obj->m_editorLayer = 1;
}

void LayoutGeneratorLayer::placeJumpIndicator(CCPoint pos, int state)
{
    bool isFlying = state & PoolState::FLYING;
    bool isMini = state & PoolState::SIZE_MINI;
    int sign = state & PoolState::GRAVITY_REVERSE ? -1 : 1;

    auto obj = createObject(
        isFlying ? ObjectId::JUMP_INDICATOR_FLYING : ObjectId::JUMP_INDICATOR_GROUNDED,
        pos + CCPoint{0.f, isFlying || isMini ? 0.f : -5.f * sign});

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

    // i can't wait for this to cause some obscure crash
    m_lastPlacedJumpIndicator = obj;
}

void LayoutGeneratorLayer::placeLabel(std::string text, CCPoint pos)
{
    auto textObj = static_cast<TextGameObject *>(createObject(ObjectId::TEXT, pos));
    textObj->updateCustomScaleX(0.25);
    textObj->updateCustomScaleY(0.25);
    textObj->m_editorLayer = 1;
    textObj->m_zLayer = ZLayer::T2;
    textObj->updateTextObject(text, false);
}

void LayoutGeneratorLayer::placeSpikeBoundary(
    bool bottom,
    CCPoint bottomPos,
    bool top,
    CCPoint topPos,
    const PlayerTrailData &midTrail,
    float dedupDistance)
{
    // wave (slopes) (unused)
    //         auto bottomSpike = createObject(objectId, spikeBottomPos);
    //         if (rightPos.y < midPos.y)
    //             bottomSpike->setRotation(90.f);
    //         else
    //         {
    //             bottomSpike->setFlipX(true);
    //             bottomSpike->setRotation(-90.f);
    //         }
    //         auto topSpike = createObject(objectId, spikeTopPos);
    //         if (rightPos.y > midPos.y)
    //         {
    //             topSpike->setFlipY(true);
    //             topSpike->setRotation(-90.f);
    //         }
    //         else
    //             topSpike->setRotation(-90.f);

    // player height + spike height - 1
    const float verticalFillDist = midTrail.rectSize.height + 11.f;

    // bottom
    if (getObjectNearPoint(bottomPos, dedupDistance, ObjectId::SPIKE) == nullptr)
    {
        if (bottom)
        {
            placeSpikeInBounds(bottomPos, midTrail, false);
            if (m_lastSpikeBottomPos.x > 0)
            {
                CCPoint bottomPos2 = bottomPos;
                while (bottomPos2.y > m_lastSpikeBottomPos.y + verticalFillDist)
                {
                    bottomPos2.y -= verticalFillDist;
                    placeSpikeInBounds(bottomPos2, midTrail, false);
                }
                while (bottomPos.y < m_lastSpikeBottomPos.y - verticalFillDist)
                {
                    m_lastSpikeBottomPos.y -= verticalFillDist;
                    placeSpikeInBounds(m_lastSpikeBottomPos, midTrail, false);
                }
            }
        }
        m_lastSpikeBottomPos = bottomPos;
    }

    // top
    if (getObjectNearPoint(topPos, dedupDistance, ObjectId::SPIKE) == nullptr)
    {
        if (top)
        {
            placeSpikeInBounds(topPos, midTrail, true);
            if (m_lastSpikeTopPos.x > 0)
            {
                while (topPos.y > m_lastSpikeTopPos.y + verticalFillDist)
                {
                    m_lastSpikeTopPos.y += verticalFillDist;
                    placeSpikeInBounds(m_lastSpikeTopPos, midTrail, true);
                }
                CCPoint topPos2 = topPos;
                while (topPos2.y < m_lastSpikeTopPos.y - verticalFillDist)
                {
                    topPos2.y += verticalFillDist;
                    placeSpikeInBounds(topPos2, midTrail, true);
                }
            }
        }
        m_lastSpikeTopPos = topPos;
    }
}

void LayoutGeneratorLayer::placeSpikeInBounds(CCPoint pos, const PlayerTrailData &trail, bool flipY)
{
    if (!isOutOfBounds(pos.y, 12.f, trail))
    {
        if (auto spike = createObject(ObjectId::SPIKE, pos))
        {
            spike->setFlipY(flipY);
        }
    }
}

bool LayoutGeneratorLayer::doesRectInterfereWithTrail(CCRect rect, float playerX, bool isBlock, bool isMini)
{
    for (auto it = m_playerTrail.rbegin(); it != m_playerTrail.rend(); ++it)
    {
        auto trail = *it;
        if (trail.pos.x > playerX - 10.f)
            continue;
        if (trail.pos.x < playerX - 45.f)
            break;

        auto playerRect = CCRect(
            trail.pos.x - trail.rectSize.width / 2.f,
            trail.pos.y - trail.rectSize.height / 2.f,
            trail.rectSize.width,
            trail.rectSize.height);

        // shrink slightly if it's a block because of the inner hitbox tolerance
        if (isBlock)
            playerRect.inflateRect(isMini ? -2.f : -4.f);

        if (playerRect.intersectsRect(rect))
        {
            log::debug("interfere x = {}", trail.pos.x - playerX);
            return true;
        }
    }

    return false;
}

// fill in the gaps in a trail when a spider pad/ring is hit
void LayoutGeneratorLayer::fillSpiderTrail(CCPoint targetPos)
{
    if (m_playerTrail.empty())
        return;

    const float fillDistance = 30.f;

    CCPoint pos = m_playerTrail.back().pos;
    while (abs(pos.y - targetPos.y) > fillDistance)
    {
        pos.y += fillDistance * (pos.y > targetPos.y ? -1 : 1);

        PlayerTrailData spiderFillTrail(m_playerTrail.back());
        spiderFillTrail.pos = pos;
        if (spiderFillTrail.state & PoolState::GROUNDED)
        {
            spiderFillTrail.state &= ~PoolState::GROUNDED;
            spiderFillTrail.state |= PoolState::AIRBORNE;
        }

        m_playerTrail.push_back(spiderFillTrail);

        if (Mod::get()->getSettingValue<bool>("debug-trail"))
            placeDebugTrailClicking(spiderFillTrail.pos, spiderFillTrail.isClicking);
    }
}

bool LayoutGeneratorLayer::isOutOfBounds(float y, float height, bool hasUpperBound, float boundsCeil, float boundsFloor)
{
    return y + height / 2.f <= boundsFloor || (y - height / 2.f >= boundsCeil && hasUpperBound);
}

bool LayoutGeneratorLayer::isOutOfBounds(float y, float height, PlayerData *pd)
{
    return isOutOfBounds(y, height, pd->state & PoolState::HAS_BOUNDS && !pd->isCameraFree(), m_boundsCeil, m_boundsFloor);
}

bool LayoutGeneratorLayer::isOutOfBounds(float y, float height, const PlayerTrailData &trail)
{
    return isOutOfBounds(y, height, trail.state & PoolState::HAS_BOUNDS && !trail.isCameraFree(), trail.boundsCeil, trail.boundsFloor);
}

GameObject *LayoutGeneratorLayer::getObjectNearPoint(CCPoint point, float radius, int objectId)
{
    auto editor = LevelEditorLayer::get();
    auto length = editor->m_objects->count();

    // only iterate over the most recent objects for efficiency
    for (int i = length - 1; i > std::max(0, (int)length - 100); i--)
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

    // editor->m_sections only works for onscreen objects and seems to contain garbage data sometimes

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

bool LayoutGeneratorLayer::getIsBuilding()
{
    return m_isBuilding;
}

void LayoutGeneratorLayer::onBuildButton(CCObject *)
{
    if (m_isBuilding)
        buildStop();
    else
    {
        if (LevelEditorLayer::get()->m_isPlatformer && !m_hasShownPlatformerWarning)
        {
            geode::createQuickPopup(
                "Warning",
                "Layout Generator is not designed for platformer mode. It may produce unexpected results.",
                "Abort",
                "Continue",
                [this](FLAlertLayer *popup, bool btn2)
                {
                    if (btn2)
                    {
                        m_hasShownPlatformerWarning = true;
                        buildStart();
                    }
                });
        }
        else
            buildStart();
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
    //             placeLabel(std::format("{},{}", std::max(0, (int)(pos.x / 100)), std::max(0, (int)(pos.y / 100))),
    //                        CCPoint{pos.x, pos.y - 7.5f});
    //         }
    //     }
    // }

    geode::openSettingsPopup(Mod::get(), false);
}

void LayoutGeneratorLayer::playtestStopped()
{
    m_isBuilding = false;

    // add undo steps for creating and selecting the generated objects
    auto editor = LevelEditorLayer::get();
    auto undo = CCArray::create();
    for (auto object : CCArrayExt<GameObject *>(editor->m_objects))
    {
        if (object && m_placedObjectUniqueIds.contains(object->m_uniqueID))
            undo->addObject(object);
    }
    if (undo->count() > 0)
    {
        editor->m_undoObjects->addObject(UndoObject::createWithArray(undo, UndoCommand::Paste));
        editor->m_undoObjects->addObject(UndoObject::createWithArray(undo, UndoCommand::Select));
        editor->m_editorUI->updateButtons();
    }

    // just to be safe
    m_placedObjectUniqueIds.clear();
}
