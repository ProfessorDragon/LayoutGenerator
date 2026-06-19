#pragma once

#include "../PoolObject/PoolEnums.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class PlayerData;
class PlayerTrailData;
class PoolObject;

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

	int m_halfBeatCount = 0;

	bool m_hasTappedThisGamemode = false;

	bool m_isClickingLastFrame = false;

	const PoolObject *m_lastPlacedFish = nullptr;

	CCPoint m_lastPlacedFishPos;

	PoolState m_lastPlayerGamemode = PoolState::GAMEMODE_CUBE;

	CCPoint m_lastSpikeBottomPos;

	CCPoint m_lastSpikeTopPos;

	int m_placeAgainTimer = -1;

	bool m_placedJumpIndicatorLastFrame = false;

	std::vector<PlayerTrailData> m_playerTrail;

	PoolTap m_shouldTap = PoolTap::NO;

	int m_shouldTapTimer = -1;

	float m_tapBalance = 0.f;

protected:
	bool init() override;

	void reset();

	// start building
	void buildStart();

	// stop building (via the EditorUI, which later calls playtestStopped)
	void buildStop();

	void update(float dt) override;

	const PoolObject *fishLegally(PlayerData *pd, int excludeTags, int requireTap);

	void placeFish(PlayerData *pd, const PoolObject *fish, bool dedup = false, bool useLastY = false);

	void placeCreditText(std::string text, CCPoint pos);

	void placeDBlock(CCPoint pos);

	void placeDebugTrailBar(CCPoint pos);

	void placeDebugTrailClicking(CCPoint pos, bool isClicking);

	void placeJumpIndicator(CCPoint pos, int state);

	void placeLabel(std::string text, CCPoint pos);

	void placeSpikeBoundary(
		bool bottom,
		CCPoint bottomPos,
		bool top,
		CCPoint topPos,
		const PlayerTrailData &midTrail,
		float dedupDistance);

	void placeSpikeInBounds(CCPoint pos, const PlayerTrailData &trail, bool flipY);

	bool doesRectInterfereWithTrail(CCRect rect, float playerX, bool isBlock, bool isMini);

	bool isOutOfBounds(float y, float height, bool hasUpperBound, float boundsCeil, float boundsFloor);

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
