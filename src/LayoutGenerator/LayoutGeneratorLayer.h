#pragma once
#include "../PoolObject/PoolEnums.h"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class PoolObject;

struct PlayerTrailData
{
	CCPoint pos;

	CCPoint vel;

	float rectWidth;

	int state;

	float boundsCeil;

	float boundsFloor;
};

class LayoutGeneratorLayer : public CCLayer
{
public:
	static LayoutGeneratorLayer *create();

protected:
	bool m_isBuilding = false;

	float m_boundsCeil = 0.f;

	float m_boundsFloor = 0.f;

	bool m_canPlaceNextFrame = false;

	float m_elapsedTime = 0.f;

	int m_fishId = 0;

	bool m_hasTappedThisGamemode = false;

	bool m_isClickingLastFrame = false;

	CCPoint m_lastGamemodePortalPos;

	const PoolObject *m_lastPlacedFish;

	CCPoint m_lastPlacedFishPos;

	PoolState m_lastPlayerGamemode = PoolState::GAMEMODE_CUBE;

	CCPoint m_lastSpikeBottomPos;

	CCPoint m_lastSpikeTopPos;

	int m_placeAgainTimer = -1;

	std::vector<PlayerTrailData> m_playerTrail;

	PoolTap m_shouldTap = PoolTap::NO;

	int m_shouldTapTimer = -1;

	float m_tapBalance = 0.f;

protected:
	bool init() override;

	void reset();

	void update(float dt) override;

	const PoolObject *fishLegally(int excludeTags, int requireTap);

	void placeFish(const PoolObject *fish, bool dedup = false, bool useLastY = false);

	void placeJumpIndicator(CCPoint pos, bool isUpsideDown, bool isFlying);

	void placeLabel(std::string text, CCPoint pos);

	void placeSpikeBoundary(
		CCPoint spikeBottomPos,
		CCPoint spikeTopPos,
		CCPoint leftPos,
		CCPoint midPos,
		CCPoint rightPos,
		int midState,
		float dedupDistance);

	void placeSpikeInBounds(CCPoint pos, bool hasBounds, bool flipY);

	bool isClicking(PlayerObject *player);

	bool isOutOfBounds(float y, float height, bool hasUpperBound);

	GameObject *getObjectNearPoint(CCPoint point, float radius, int objectId = -1);

	CCRect getObjectRect(GameObject *obj);

	PoolState getPlayerGamemode(PlayerObject *player);

	int getPlayerState(PlayerObject *player);

public:
	void onBuildButton(CCObject *);

	void onSettingsButton(CCObject *);

	void playtestStopped();
};
