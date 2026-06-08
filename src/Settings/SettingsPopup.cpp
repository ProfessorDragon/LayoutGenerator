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

    this->setTitle("Level Generator");

    m_bpmInput = TextInput::create(50.f, ZStringView());
    m_bpmInput->setCallback(
        [settings](const std::string &value)
        {
            auto result = utils::numFromString<float>(value);
            if (result.isOk())
                settings->setBpm(result.unwrap());
        });
    std::string s = std::to_string(settings->getBpm());
    while (s.ends_with("0") || s.ends_with("."))
        s = s.substr(0, s.size() - 1);
    m_bpmInput->setString(s);
    m_bpmInput->setFilter("1234567890.");
    m_bpmInput->setLabel("BPM");
    m_bpmInput->setMaxCharCount(8);
    m_bpmInput->focus();
    m_mainLayer->addChildAtPosition(m_bpmInput, Anchor::Center, CCPoint{0.f, 25.f});

    auto label = CCLabelBMFont::create(
        "Examples:\n"
        "Creo - In Circles (786863) - 92bpm\n"
        "Creo - Lightmare (914838) - 108bpm\n"
        "ConnorGrail - What Is It You Seek? (1286522) - 108bpm\n"
        "Creo - Ballistic Funk (905109) - 113bpm\n"
        "Acid-Notation - The Yandere's Puppet Show (722366) - 128bpm\n"
        "meganeko - Milkshake (684652) - 128bpm\n"
        "EnV - Uprise (513064) - 130bpm",
        "bigFont.fnt");
    label->setScale(0.3f);
    m_mainLayer->addChildAtPosition(label, Anchor::Center, CCPoint{0.f, -50.f});

    return true;
}
