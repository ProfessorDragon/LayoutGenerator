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
        std::vector<PlayerTrailData> m_queuedTrail;
    };

    // void update(float dt)
    // {
    //     PlayerObject::update(dt);
    // }

    void updateJump(float dt)
    {
        if (auto editor = static_cast<MyLevelEditorLayer *>(LevelEditorLayer::get()))
        {
            auto builder = editor->m_fields->m_builder;
            if (builder && builder->getIsBuilding())
            {
                auto pd = new PlayerData();
                pd->setPlayer(this);

                auto trail = PlayerTrailData::fromPlayerData(*pd);

                // jump indicators v3
                if (m_isBird || m_isSwing)
                {
                    if (m_stateRingJump && m_jumpBuffered && !m_isDashing)
                        trail.makeJumpIndicator = true;
                }
                // cube, ball, robot, spider
                else if (!m_isShip && !m_isDart)
                {
                    if (m_isOnGround && m_jumpBuffered && (!m_isRobot || m_stateRingJump) && !m_isDashing)
                        trail.makeJumpIndicator = true;
                }

                // btw i hate robtop for calling updateJump from update AND pushButton
                m_fields->m_queuedTrail.push_back(trail);
            }
        }

        PlayerObject::updateJump(dt);
    }
};
