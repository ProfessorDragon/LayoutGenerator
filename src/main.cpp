#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include "LayoutGenerator/LayoutGeneratorLayer.h"

using namespace geode::prelude;

// LevelEditorLayer

#include <Geode/modify/LevelEditorLayer.hpp>
class $modify(MyLevelEditorLayer, LevelEditorLayer)
{
	struct Fields
	{
		LayoutGeneratorLayer *m_builder = nullptr;
	};

	bool init(GJGameLevel *level, bool noUI)
	{
		// create builder before init, so EditorUI has access to the builder in its init
		auto builder = LayoutGeneratorLayer::create();
		addChild(builder);
		builder->setID("builder"_spr);
		m_fields->m_builder = builder;

		if (!LevelEditorLayer::init(level, noUI))
			return false;

		return true;
	}
};

// EditorUI

#include <Geode/modify/EditorUI.hpp>
class $modify(MyEditorUI, EditorUI)
{
	LayoutGeneratorLayer *getBuilder()
	{
		return static_cast<MyLevelEditorLayer *>(m_editorLayer)->m_fields->m_builder;
	}

	bool init(LevelEditorLayer *editorLayer)
	{
		if (!EditorUI::init(editorLayer))
			return false;

		auto buildSprite = CircleButtonSprite::createWithSpriteFrameName("button.png"_spr, 1.2f, CircleBaseColor::Blue);
		buildSprite->setScale(0.83f);
		auto buildButton = CCMenuItemSpriteExtra::create(
			buildSprite,
			getBuilder(),
			menu_selector(LayoutGeneratorLayer::onBuildButton));
		buildButton->setID("build-button"_spr);

		auto settingsSprite = CCSprite::createWithSpriteFrameName("GJ_BPMOnBtn_001.png");
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

// $on_game(Loaded){
// 	auto levelManager = LocalLevelManager::get();
// 	if (!levelManager)
// 	{
// 		log::error("Failed to get LocalLevelManager instance.");
// 		return;
// 	}

// 	if (levelManager->m_localLevels)
// 	{
// 		auto level = static_cast<GJGameLevel *>(levelManager->m_localLevels->objectAtIndex(0));
// 		auto scene = LevelEditorLayer::scene(level, false);
// 		CCDirector::sharedDirector()->pushScene(scene);
// 	}
// };
