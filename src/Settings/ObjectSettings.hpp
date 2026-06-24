#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ObjectSettings : public SettingBaseValueV3<std::vector<int>>
{
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const &key, std::string const &modID, matjson::Value const &json);

    SettingNodeV3 *createNode(float width) override;
};

template <>
struct geode::SettingTypeForValueType<std::vector<int>>
{
    using SettingType = ObjectSettings;
};

class ObjectSettingsNode : public SettingValueNodeV3<ObjectSettings>
{
public:
    static ObjectSettingsNode *create(std::shared_ptr<ObjectSettings> setting, float width);

protected:
    CCMenu *m_toggleMenu = nullptr;

    std::vector<CCMenuItemToggler *> m_toggles;

    bool init(std::shared_ptr<ObjectSettings> setting, float width);

    void updateState(CCNode *invoker) override;

    void onToggle(CCObject *sender);
};
