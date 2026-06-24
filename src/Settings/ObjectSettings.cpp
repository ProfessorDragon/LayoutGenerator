#include "ObjectSettings.hpp"
#include "../GameObjectPool/GameObjectPool.hpp"

Result<std::shared_ptr<SettingV3>>
ObjectSettings::parse(std::string const &key, std::string const &modID, matjson::Value const &json)
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

const std::vector<std::vector<int>> ObjectSettingsNode::OBJECT_ID_LAYOUT = {
    // gamemode, speed, size portals
    {12, 13, 47, 111, 660, 745, 1331, 1933, 10, 11, 99, 101},
    // speed portals and pads
    {200, 201, 202, 203, 1334, 35, 140, 1332, 67, 3005},
    // rings
    {36, 141, 1333, 84, 1022, 1330, 3004, 1704, 1751},
};

const std::unordered_set<int> ObjectSettingsNode::OBJECT_ID_WHITELISTABLE = []()
{
    std::unordered_set<int> w = {};

    for (auto &row : OBJECT_ID_LAYOUT)
        for (auto key : row)
            w.insert(key);

    return w;
}();

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

    const float height = 160.f;

    setContentSize({width, height});

    getNameMenu()->setContentWidth(width);
    getNameMenu()->setLayout(RowLayout::create()
                                 ->setAxisAlignment(AxisAlignment::Start)
                                 ->setAutoScale(false));
    getNameMenu()->setLayoutOptions(AnchorLayoutOptions::create()
                                        ->setAnchor(Anchor::TopLeft)
                                        ->setOffset({10.f, -15.f}));

    m_toggleMenu = CCMenu::create();
    m_toggleMenu->setContentSize({width, height});
    m_toggleMenu->setLayout(ColumnLayout::create()
                                ->setAxisReverse(true));

    for (auto &row : OBJECT_ID_LAYOUT)
    {
        auto menu = CCMenu::create();
        menu->setContentWidth(width);
        menu->setLayout(RowLayout::create()
                            ->setAutoScale(false));

        for (int j = 0; j < row.size(); j++)
        {
            auto key = row[j];
            auto frame = ObjectToolbox::sharedState()->intKeyToFrame(key);

            auto offSprite = GameObject::createWithSpriteFrameName(frame);
            offSprite->setOpacity(90);
            offSprite->setScale(offSprite->getContentHeight() > 45.f ? .5f : .75f);
            if (offSprite->getContentHeight() < 20.f)
                offSprite->setContentHeight(20.f);

            auto onSprite = GameObject::createWithSpriteFrameName(frame);
            onSprite->setScale(onSprite->getContentHeight() > 45.f ? .5f : .75f);
            if (onSprite->getContentHeight() < 20.f)
                onSprite->setContentHeight(20.f);

            auto toggle = CCMenuItemToggler::create(
                offSprite,
                onSprite,
                this,
                menu_selector(ObjectSettingsNode::onToggle));
            toggle->m_notClickable = true;
            toggle->setTag(key);

            m_toggles.push_back(toggle);
            menu->addChild(toggle);
        }

        menu->updateLayout();
        m_toggleMenu->addChild(menu);
    }

    m_toggleMenu->ignoreAnchorPointForPosition(false);
    m_toggleMenu->updateLayout();
    addChildAtPosition(m_toggleMenu, Anchor::Center, {0.f, -15.f});

    updateState(nullptr);

    return true;
}

void ObjectSettingsNode::updateState(CCNode *invoker)
{
    SettingValueNodeV3::updateState(invoker);

    auto shouldEnable = getSetting()->shouldEnable();

    auto value = getValue();

    for (auto toggle : m_toggles)
    {
        toggle->toggle(value.contains(toggle->getTag()));
        toggle->setEnabled(shouldEnable);
        toggle->setCascadeColorEnabled(true);
        toggle->setCascadeOpacityEnabled(true);
        toggle->setOpacity(shouldEnable ? 255 : 155);
        toggle->setColor(shouldEnable ? ccWHITE : ccGRAY);
    }
}

void ObjectSettingsNode::onToggle(CCObject *sender)
{
    auto value = getValue();

    if (value.contains(sender->getTag()))
        value.erase(sender->getTag());
    else
        value.insert(sender->getTag());

    setValue(value, static_cast<CCNode *>(sender));
}
