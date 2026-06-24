#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ObjectSettings : public SettingBaseValueV3<std::unordered_set<int>>
{
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const &key, std::string const &modID, matjson::Value const &json);

    SettingNodeV3 *createNode(float width) override;
};

template <>
struct geode::SettingTypeForValueType<std::unordered_set<int>>
{
    using SettingType = ObjectSettings;
};

class ObjectSettingsNode : public SettingValueNodeV3<ObjectSettings>
{
protected:
    static const std::vector<std::vector<int>> OBJECT_ID_LAYOUT;

public:
    static const std::unordered_set<int> OBJECT_ID_WHITELISTABLE;

public:
    static ObjectSettingsNode *create(std::shared_ptr<ObjectSettings> setting, float width);

protected:
    CCMenu *m_toggleMenu = nullptr;

    std::vector<CCMenuItemToggler *> m_toggles;

    bool init(std::shared_ptr<ObjectSettings> setting, float width);

    void updateState(CCNode *invoker) override;

    void onToggle(CCObject *sender);
};
