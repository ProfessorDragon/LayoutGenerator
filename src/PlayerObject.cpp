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
    }

    void updateJump(float dt)
    {
        if (m_isBird || m_isSwing)
        {
            if ((m_stateRingJump || m_isOnGround) && m_jumpBuffered)
                m_fields->m_makeJumpIndicator = 2;
        }
        // cube, ball, robot, spider
        else if (!m_isShip && !m_isDart)
        {
            if (m_isOnGround && m_jumpBuffered)
                m_fields->m_makeJumpIndicator = 2;
        }

        PlayerObject::updateJump(dt);
    }
};
