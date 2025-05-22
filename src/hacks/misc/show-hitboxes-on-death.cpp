#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

static void removeNode(CCNode* node) {
    node->removeFromParentAndCleanup(true);
}

class $modify(qol, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* cause) {
        PlayLayer::destroyPlayer(player, cause);

        if (!Mod::get()->getSettingValue<bool>("show-hitbox")) return;

        auto rect = player->getObjectRect();
        auto box = CCLayerColor::create(ccc4(255, 0, 0, 100), rect.size.width, rect.size.height);
        box->setPosition(rect.origin);
        box->setZOrder(9999);

        box->runAction(CCSequence::create(
            CCFadeOut::create(2.0f),
            CCCallFuncN::create(removeNode),
            nullptr
        ));

        this->addChild(box);
    }
};