#include "ObjectSettings.hpp"

Result<std::shared_ptr<SettingV3>> ObjectSettings::parse(std::string const &key, std::string const &modID, matjson::Value const &json)
{
    auto res = std::make_shared<ObjectSettings>();
    auto root = checkJson(json, "ObjectSettings");
    res->parseBaseProperties(key, modID, root);
    root.checkUnknownKeys();

    return root.ok(std::static_pointer_cast<SettingV3>(res));
}

SettingNodeV3 *ObjectSettings::createNode(float width)
{
    return ObjectSettingsNode::create(std::static_pointer_cast<ObjectSettings>(shared_from_this()), width);
}

ObjectSettingsNode *ObjectSettingsNode::create(std::shared_ptr<ObjectSettings> setting, float width)
{
    auto ret = new ObjectSettingsNode();
    if (ret->init(setting, width))
    {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ObjectSettingsNode::init(std::shared_ptr<ObjectSettings> setting, float width)
{
    if (!SettingValueNodeV3::init(setting, width))
        return false;

    for (auto value : {"GJ_starsIcon_001.png", "currencyOrbIcon_001.png"})
    {
        auto offSpr = CCSprite::createWithSpriteFrameName(value);
        offSpr->setOpacity(90);
        auto onSpr = CCSprite::createWithSpriteFrameName(value);
        auto toggle = CCMenuItemToggler::create(
            offSpr,
            onSpr,
            this,
            menu_selector(ObjectSettingsNode::onToggle));
        toggle->m_notClickable = true;
        // toggle->setTag(value);
        m_toggles.push_back(toggle);

        getButtonMenu()->addChild(toggle);
    }

    getButtonMenu()->setContentWidth(40);
    getButtonMenu()->setLayout(RowLayout::create());

    updateState(nullptr);

    return true;
}

void ObjectSettingsNode::updateState(CCNode *invoker)
{
    SettingValueNodeV3::updateState(invoker);

    auto shouldEnable = getSetting()->shouldEnable();

    for (auto toggle : m_toggles)
    {
        // toggle->toggle(getValue().contains(toggle->getTag()));

        toggle->setEnabled(shouldEnable);
        toggle->setCascadeColorEnabled(true);
        toggle->setCascadeOpacityEnabled(true);
        toggle->setOpacity(shouldEnable ? 255 : 155);
        toggle->setColor(shouldEnable ? ccWHITE : ccGRAY);
    }
}

void ObjectSettingsNode::onToggle(CCObject *sender)
{
    // setValue(
    //     static_cast<MyCustomEnum2>(sender->getTag()),
    //     static_cast<CCNode *>(sender));
}
