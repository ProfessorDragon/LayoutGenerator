#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/PlayerObject.hpp>
class $modify(MyPlayerObject, PlayerObject)
{
    struct Fields
    {
        int m_makeJumpIndicator = 0;
    };

    void update(float dt)
    {
        m_fields->m_makeJumpIndicator--;

        PlayerObject::update(dt);

        // this breaks some things
        // if (auto editor = static_cast<MyLevelEditorLayer *>(LevelEditorLayer::get()))
        // {
        //     if (auto builder = editor->m_fields->m_builder)
        //     {
        //         builder->update(dt);
        //     }
        // }
    }

    void updateJump(float dt)
    {
        if (m_isBird || m_isSwing)
        {
            if (m_stateRingJump && m_jumpBuffered && !m_isDashing)
                m_fields->m_makeJumpIndicator = 2;
        }
        // cube, ball, robot, spider
        else if (!m_isShip && !m_isDart)
        {
            if (m_isOnGround && m_jumpBuffered && (!m_isRobot || m_stateRingJump) && !m_isDashing)
                m_fields->m_makeJumpIndicator = 2;
        }

        PlayerObject::updateJump(dt);
    }
};
