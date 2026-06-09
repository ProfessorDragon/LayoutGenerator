#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class Settings;

class SettingsPopup : public Popup
{
public:
    static SettingsPopup *create(Settings *settings);

protected:
    Settings *m_settings;

protected:
    bool init(Settings *settings);

    CCMenu *createCheckbox(char const *label, bool initialValue, cocos2d::CCObject *target, cocos2d::SEL_MenuHandler callback);

    void onCheckboxDontChangeGamemode(CCObject *sender);

    void onCheckboxMakeDebugTrail(CCObject *sender);

    void onCheckboxUsePlayerClicks(CCObject *sender);
};
