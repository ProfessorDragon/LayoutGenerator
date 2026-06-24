#include <Geode/Geode.hpp>
#include "Settings/ObjectSettings.hpp"

using namespace geode::prelude;

$on_mod(Loaded)
{
    Mod::get()->registerCustomSettingType("objects", &ObjectSettings::parse);
}

// $on_game(Loaded)
// {
//     auto levelManager = LocalLevelManager::get();
//     if (!levelManager)
//     {
//         log::error("Failed to get LocalLevelManager instance.");
//         return;
//     }

//     if (levelManager->m_localLevels)
//     {
//         auto level = static_cast<GJGameLevel *>(levelManager->m_localLevels->objectAtIndex(0));
//         auto scene = LevelEditorLayer::scene(level, false);
//         CCDirector::sharedDirector()->pushScene(scene);
//     }
// };
