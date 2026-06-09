#include <Geode/Geode.hpp>
#include "LayoutGenerator/LayoutGeneratorLayer.h"

using namespace geode::prelude;

// class $modify(PlayerObject) {
//     void pushButton(PlayerButton button) {
//         PlayerObject::pushButton(button);
//     }
// };

// LevelEditorLayer

#include <Geode/modify/LevelEditorLayer.hpp>
class $modify(MyLevelEditorLayer, LevelEditorLayer)
{
	struct Fields
	{
		LayoutGeneratorLayer *m_builder;
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
	struct Fields
	{
		CCMenuItemSpriteExtra *m_buildButton;

		CCMenuItemSpriteExtra *m_settingsButton;
	};

	LayoutGeneratorLayer *getBuilder()
	{
		return static_cast<MyLevelEditorLayer *>(m_editorLayer)->m_fields->m_builder;
	}

	bool init(LevelEditorLayer *editorLayer)
	{
		if (!EditorUI::init(editorLayer))
			return false;

		auto buildSprite = CCSprite::createWithSpriteFrameName("GJ_everyplayBtn_001.png");
		buildSprite->setScale(0.75f);
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
		m_fields->m_buildButton = buildButton;
		m_fields->m_settingsButton = settingsButton;
		menu->updateLayout();

		return true;
	}

	/**
	 * playtest is started from the playtest button or build button
	 */
	void onPlaytest(CCObject *sender)
	{
		EditorUI::onPlaytest(sender);
		m_fields->m_buildButton->setVisible(false);
		m_fields->m_settingsButton->setVisible(false);
	}

	/**
	 * playtest is stopped from the playtest button
	 */
	void onStopPlaytest(CCObject *sender)
	{
		EditorUI::onStopPlaytest(sender);
	}

	/**
	 * playtest is stopped from the playtest button, player death, or build button
	 */
	void playtestStopped()
	{
		EditorUI::playtestStopped();
		m_fields->m_buildButton->setVisible(true);
		m_fields->m_settingsButton->setVisible(true);
		getBuilder()->playtestStopped();
	}
};

$on_game(Loaded){
	// auto levelManager = LocalLevelManager::get();
	// if (!levelManager)
	// {
	// 	log::error("Failed to get LocalLevelManager instance.");
	// 	return;
	// }

	// if (levelManager->m_localLevels)
	// {
	// 	auto level = static_cast<GJGameLevel *>(levelManager->m_localLevels->objectAtIndex(0));
	// 	auto scene = LevelEditorLayer::scene(level, false);
	// 	CCDirector::sharedDirector()->pushScene(scene);
	// }
};
