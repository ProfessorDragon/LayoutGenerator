#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class Settings;

class SettingsPopup : public Popup
{
public:
    static SettingsPopup *create(Settings *settings);

protected:
    TextInput *m_bpmInput;

protected:
    bool init(Settings *settings);
};
