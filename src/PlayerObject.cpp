#pragma once

#include <Geode/Geode.hpp>
#include "PlayerData/PlayerData.hpp"
#include "LevelEditorLayer.cpp"

using namespace geode::prelude;

#include <Geode/modify/PlayerObject.hpp>
class $modify(MyPlayerObject, PlayerObject)
{
    struct Fields
    {
        bool m_makeJumpIndicator = false;

        std::vector<PlayerTrailData> m_queuedTrail;
    };

    void update(float dt)
    {
        bool didPush = false;

        if (auto editor = static_cast<MyLevelEditorLayer *>(LevelEditorLayer::get()))
        {
            auto builder = editor->m_fields->m_builder;
            if (builder && builder->getIsBuilding())
            {
                auto pd = new PlayerData();
                pd->setPlayer(this);

                auto trail = PlayerTrailData::fromPlayerData(*pd);
                m_fields->m_queuedTrail.push_back(trail);
                didPush = true;
            }
        }

        m_fields->m_makeJumpIndicator = false;

        PlayerObject::update(dt);

        if (didPush)
            m_fields->m_queuedTrail.back().makeJumpIndicator = m_fields->m_makeJumpIndicator;
    }

    void updateJump(float dt)
    {
        if (m_isBird || m_isSwing)
        {
            if (m_stateRingJump && m_jumpBuffered && !m_isDashing)
                m_fields->m_makeJumpIndicator = true;
        }
        // cube, ball, robot, spider
        else if (!m_isShip && !m_isDart)
        {
            if (m_isOnGround && m_jumpBuffered && (!m_isRobot || m_stateRingJump) && !m_isDashing)
                m_fields->m_makeJumpIndicator = true;
        }

        PlayerObject::updateJump(dt);
    }
};
