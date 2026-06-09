#include "SettingsPopup.h"
#include "Settings.h"

SettingsPopup *SettingsPopup::create(Settings *settings)
{
    auto ret = new SettingsPopup();
    if (ret->init(settings))
    {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool SettingsPopup::init(Settings *settings)
{
    if (!Popup::init(340.f, 200.f))
        return false;

    m_settings = settings;

    this->setTitle("Level Generator");

    auto info = InfoAlertButton::create(
        "Info",
        "<cs>BPM</c>: Enter the song's BPM to make gameplay sync.\n"
        "<cs>Use player clicks</c>: If enabled, the gameplay will form around your clicks. "
        "If not, clicks will be inserted at random.\n"
        "<cs>Don't change gamemode</c>: Prevents gamemode portals from spawning.",
        .5f);
    info->setID("info"_spr);
    // WHYYYY DOES THIS NEED TO BE ADDED ON A SEPARATE LAYER????
    m_buttonMenu->addChildAtPosition(info, Anchor::TopRight, CCPoint{-15.f, -15.f});

    auto bpm = TextInput::create(50.f, ZStringView());
    bpm->setCallback(
        [settings](const std::string &value)
        {
            auto result = utils::numFromString<float>(value);
            if (result.isOk())
                settings->setBpm(result.unwrap());
        });
    std::string s = std::to_string(settings->getBpm());
    while (s.ends_with("0") || s.ends_with("."))
        s = s.substr(0, s.size() - 1);
    bpm->setString(s);
    bpm->setFilter("1234567890.");
    bpm->setLabel("BPM");
    bpm->setMaxCharCount(8);
    bpm->focus();
    bpm->setID("bpm-input"_spr);
    m_mainLayer->addChildAtPosition(bpm, Anchor::Center, CCPoint{-110.f, 25.f});

    auto usePlayerClicks = createCheckbox(
        "Use player clicks",
        settings->getUsePlayerClicks(),
        this,
        menu_selector(SettingsPopup::onCheckboxUsePlayerClicks));
    usePlayerClicks->setID("use-player-clicks"_spr);
    m_mainLayer->addChildAtPosition(usePlayerClicks, Anchor::Center, CCPoint{185.f, 45.f});

    auto dontChangeGamemode = createCheckbox(
        "Dont change gamemode",
        settings->getExcludeTags() & PoolTag::GAMEMODE,
        this,
        menu_selector(SettingsPopup::onCheckboxDontChangeGamemode));
    dontChangeGamemode->setID("dont-change-gamemode"_spr);
    m_mainLayer->addChildAtPosition(dontChangeGamemode, Anchor::Center, CCPoint{185.f, 25.f});

    auto makeDebugTrail = createCheckbox(
        "Make debug trail",
        settings->getMakeDebugTrail(),
        this,
        menu_selector(SettingsPopup::onCheckboxMakeDebugTrail));
    makeDebugTrail->setID("make-debug-trail"_spr);
    m_mainLayer->addChildAtPosition(makeDebugTrail, Anchor::Center, CCPoint{185.f, 5.f});

    auto examples = CCLabelBMFont::create(
        "Examples:\n"
        "Creo - In Circles (786863) - 92bpm\n"
        "Creo - Lightmare (914838) - 107bpm\n"
        "ConnorGrail - What Is It You Seek? (1286522) - 108bpm\n"
        "Creo - Ballistic Funk (905109) - 113bpm\n"
        "Acid-Notation - The Yandere's Puppet Show (722366) - 128bpm\n"
        "meganeko - Milkshake (684652) - 128bpm\n"
        "EnV - Uprise (513064) - 130bpm",
        "bigFont.fnt");
    examples->setScale(0.3f);
    examples->setID("examples"_spr);
    m_mainLayer->addChildAtPosition(examples, Anchor::Center, CCPoint{0.f, -50.f});

    return true;
}

CCMenu *SettingsPopup::createCheckbox(char const *label, bool initialValue, cocos2d::CCObject *target, cocos2d::SEL_MenuHandler callback)
{
    auto row = CCMenu::create();
    row->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false));

    auto checkbox = CCMenuItemToggler::createWithStandardSprites(target, callback, 1.f);
    checkbox->setScale(0.5f);
    checkbox->toggle(initialValue);

    auto labelNode = CCLabelBMFont::create(label, "bigFont.fnt");
    labelNode->setScale(0.45f);

    row->addChild(checkbox);
    row->addChild(labelNode);
    row->updateLayout();

    return row;
}

void SettingsPopup::onCheckboxDontChangeGamemode(CCObject *sender)
{
    if (auto checkbox = static_cast<CCMenuItemToggler *>(sender))
    {
        m_settings->setExcludeTags(!checkbox->isToggled() ? PoolTag::GAMEMODE : 0);
    }
}

void SettingsPopup::onCheckboxMakeDebugTrail(CCObject *sender)
{
    if (auto checkbox = static_cast<CCMenuItemToggler *>(sender))
    {
        m_settings->setMakeDebugTrail(!checkbox->isToggled());
    }
}

void SettingsPopup::onCheckboxUsePlayerClicks(CCObject *sender)
{
    if (auto checkbox = static_cast<CCMenuItemToggler *>(sender))
    {
        m_settings->setUsePlayerClicks(!checkbox->isToggled());
    }
}
