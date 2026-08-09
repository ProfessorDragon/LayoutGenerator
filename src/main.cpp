#include <Geode/Geode.hpp>
#include <smjs.object-collab/include/Optionals.hpp>

#include "GameObjectPool/GameObjectPool.hpp"
#include "Settings/ObjectSettings.hpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    if (!Mod::get()->registerCustomSettingType("objects", &ObjectSettings::parse)) {
        log::error("Failed to register custom setting type");
    }
}

$on_game(Loaded) {
    std::unordered_map<std::string, uint32_t> customObjects;
    Result result = object_collab::getOptionalRegister();
    if (result) {
        log::info("Found Object Collab");
        for (auto [objectId, info] : result.unwrap()) {
            customObjects[info.id] = objectId;
        }
    }
    GameObjectPool::generate(customObjects);

    // auto levelManager = LocalLevelManager::get();
    // if (!levelManager) {
    //     log::error("Failed to get LocalLevelManager instance.");
    //     return;
    // }

    // if (levelManager->m_localLevels) {
    //     auto level = static_cast<GJGameLevel*>(levelManager->m_localLevels->objectAtIndex(0));
    //     auto scene = LevelEditorLayer::scene(level, false);
    //     CCDirector::sharedDirector()->pushScene(scene);
    // }
};
