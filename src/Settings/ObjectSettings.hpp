#include <Geode/Geode.hpp>

using namespace geode::prelude;

// template <>
// struct matjson::Serialize<MyCustomEnum2>
// {
//     static matjson::Value toJson(MyCustomEnum2 const &value)
//     {
//         switch (value)
//         {
//         default:
//         case MyCustomEnum2::ValidEnumValue:
//             return "valid-enum-value";
//         case MyCustomEnum2::OtherValidEnumValue:
//             return "other-valid-enum-value";
//         }
//     }

//     static Result<MyCustomEnum2> fromJson(matjson::Value const &value)
//     {
//         GEODE_UNWRAP_INTO(auto str, value.asString());
//         switch (hash(str))
//         {
//         case hash("valid-enum-value"):
//             return Ok(MyCustomEnum2::ValidEnumValue);
//         case hash("other-valid-enum-value"):
//             return Ok(MyCustomEnum2::OtherValidEnumValue);
//         default:
//             return Err("invalid MyCustomEnum value '{}'", str);
//         }
//     }
// };

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
    std::vector<CCMenuItemToggler *> m_toggles;

    bool init(std::shared_ptr<ObjectSettings> setting, float width);

    void updateState(CCNode *invoker) override;

    void onToggle(CCObject *sender);
};
