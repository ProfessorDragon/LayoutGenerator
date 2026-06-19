#include <Geode/Geode.hpp>
#include "LayoutGenerator/LayoutGeneratorLayer.hpp"
#include "LevelEditorLayer.cpp"

using namespace geode::prelude;

#include <Geode/modify/EditorUI.hpp>
class $modify(EditorUI)
{
	LayoutGeneratorLayer *getBuilder()
	{
		return static_cast<MyLevelEditorLayer *>(m_editorLayer)->m_fields->m_builder;
	}

	bool init(LevelEditorLayer *editorLayer)
	{
		if (!EditorUI::init(editorLayer))
			return false;

		auto buildSprite = ButtonSprite::createWithSpriteFrameName("button.png"_spr);
		buildSprite->setScale(0.83f);
		auto buildButton = CCMenuItemSpriteExtra::create(
			buildSprite,
			getBuilder(),
			menu_selector(LayoutGeneratorLayer::onBuildButton));
		buildButton->setID("build-button"_spr);

		// @geode-ignore(unknown-resource)
		auto settingsSprite = CircleButtonSprite::createWithSpriteFrameName("geode.loader/settings.png", 1.f, CircleBaseColor::DarkAqua);
		settingsSprite->setScale(0.83f);
		auto settingsButton = CCMenuItemSpriteExtra::create(
			settingsSprite,
			getBuilder(),
			menu_selector(LayoutGeneratorLayer::onSettingsButton));
		settingsButton->setID("settings-button"_spr);

		auto menu = getChildByID("playback-menu");
		menu->addChild(buildButton);
		menu->addChild(settingsButton);
		m_uiItems->addObject(buildButton);
		m_uiItems->addObject(settingsButton);
		menu->updateLayout();

		return true;
	}

	// playtest is started from the playtest button or build button
	// void onPlaytest(CCObject *sender);

	// playtest is stopped from the playtest button
	// void onStopPlaytest(CCObject *sender);

	// playtest is stopped from the playtest button, player death, or build button
	void playtestStopped()
	{
		EditorUI::playtestStopped();
		getBuilder()->playtestStopped();
	}
};
