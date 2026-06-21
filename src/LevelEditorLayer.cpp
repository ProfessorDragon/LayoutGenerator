#pragma once

#include <Geode/Geode.hpp>
#include "LayoutGenerator/LayoutGeneratorLayer.hpp"

using namespace geode::prelude;

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
