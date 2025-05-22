#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/UILayer.hpp>

using namespace geode::prelude;

class $modify (StartposPlayLayer, PlayLayer)
{
    struct Fields {
        std::vector<StartPosObject*> objs;
        int selectedIndex = -1;

        CCMenu* menu = nullptr;
        CCSprite* left = nullptr;
        CCSprite* right = nullptr;
        CCLabelBMFont* label = nullptr;

        CCPoint position;
        float scale;
        float opacity;
    };

    void setStartpos(int index)
    {
        if (m_fields->objs.empty())
            return;

        std::sort(m_fields->objs.begin(), m_fields->objs.end(), [](auto* a, auto* b) {
            return a->getPositionX() < b->getPositionX();
        });

        if (index < -1)
            index = m_fields->objs.size() - 1;

        if (index >= m_fields->objs.size())
            index = -1;

        m_fields->selectedIndex = index;

        m_currentCheckpoint = nullptr;

        auto spos = index >= 0 ? m_fields->objs[index] : nullptr;

        setStartPosObject(spos);

        if (m_isPracticeMode)
            resetLevelFromStart();

        resetLevel();
        startMusic();

        updateUI();
    }

    void onLeft(CCObject*)
    {
        setStartpos(m_fields->selectedIndex - 1);
    }

    void onRight(CCObject*)
    {
        setStartpos(m_fields->selectedIndex + 1);
    }

    void addObject(GameObject* obj)
    {
        if (obj->m_objectID == 31)
        {
            m_fields->objs.push_back(as<StartPosObject*>(obj));

            #ifndef GEODE_IS_MACOS
            if (as<StartPosObject*>(obj)->m_startSettings && !as<StartPosObject*>(obj)->m_startSettings->m_disableStartPos)
                m_fields->selectedIndex++;
            #endif
        }

        PlayLayer::addObject(obj);
    }

    void createObjectsFromSetupFinished()
    {
        PlayLayer::createObjectsFromSetupFinished();

        if (m_fields->menu)
            m_fields->menu->removeFromParent();

        m_fields->menu = CCMenu::create();
        m_fields->menu->setContentSize(ccp(0, 0));
        m_fields->menu->setPosition(CCDirector::get()->getWinSize().width / 2, 25);

        m_fields->label = CCLabelBMFont::create("1/1", "bigFont.fnt");

        m_fields->left = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        m_fields->right = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        m_fields->left->setScale(0.65f);
        m_fields->right->setScale(0.65f);

        m_fields->right->setFlipX(true);

        auto l = CCMenuItemSpriteExtra::create(m_fields->left, this, menu_selector(StartposPlayLayer::onLeft));
        auto r = CCMenuItemSpriteExtra::create(m_fields->right, this, menu_selector(StartposPlayLayer::onRight));

        m_fields->menu->addChildAtPosition(l, Anchor::Center, ccp(-65, 0));
        m_fields->menu->addChildAtPosition(r, Anchor::Center, ccp(65, 0));
        m_fields->menu->addChildAtPosition(m_fields->label, Anchor::Center);

        m_uiLayer->addChild(m_fields->menu);

        bool enabled = false;
        if (auto mod = Mod::get()->getSubmodule("startpos-switcher"))
            enabled = mod->isEnabled();

        m_fields->menu->setVisible(enabled && !m_fields->objs.empty());

        updateUI();
    }

    void updateUI()
    {
        if (!m_fields->menu || !m_fields->label)
            return;

        m_fields->position = ccp(
            Mod::get()->getSavedValue<float>("startpos-position.x", CCDirector::get()->getWinSize().width / 2),
            Mod::get()->getSavedValue<float>("startpos-position.y", 25)
        );
        m_fields->scale = Mod::get()->getSavedValue<float>("startpos-scale", 1);
        m_fields->opacity = Mod::get()->getSavedValue<float>("startpos-opacity", 75.0f / 255.0f);

        m_fields->label->setString(fmt::format("{}/{}", m_fields->selectedIndex + 1, m_fields->objs.size()).c_str());
        m_fields->label->limitLabelWidth(100, 0.65f, 0);

        auto action = CCSequence::create(
            CCFadeTo::create(0.1f, 225),
            CCFadeTo::create(0.6f, 225),
            CCFadeTo::create(0.3f, 255 * m_fields->opacity),
            nullptr
        );

        m_fields->label->runAction(action);
        m_fields->left->runAction(action->copy());
        m_fields->right->runAction(action->copy());
    }

    virtual void postUpdate(float dt) override
    {
        PlayLayer::postUpdate(dt);

        m_fields->position = ccp(
            Mod::get()->getSavedValue<float>("startpos-position.x", CCDirector::get()->getWinSize().width / 2),
            Mod::get()->getSavedValue<float>("startpos-position.y", 25)
        );
        m_fields->scale = Mod::get()->getSavedValue<float>("startpos-scale", 1);
        m_fields->opacity = Mod::get()->getSavedValue<float>("startpos-opacity", 1);

        if (m_fields->menu) {
            m_fields->menu->setPosition(m_fields->position);
            m_fields->menu->setScale(m_fields->scale);
            m_fields->menu->setOpacity(255 * m_fields->opacity);
        }
    }
};

class $modify (UILayer)
{
    #ifdef GEODE_IS_WINDOWS
    virtual void keyDown(cocos2d::enumKeyCodes key) override
    {
        UILayer::keyDown(key);

        bool enabled = false;
        if (auto mod = Mod::get()->getSubmodule("startpos-switcher"))
            enabled = mod->isEnabled();

        if (!enabled)
            return;

        if (auto pl = PlayLayer::get()) {
            auto spl = as<StartposPlayLayer*>(pl);

            if (key == enumKeyCodes::KEY_Q)
                spl->setStartpos(spl->m_fields->selectedIndex - 1);

            else if (key == enumKeyCodes::KEY_E)
                spl->setStartpos(spl->m_fields->selectedIndex + 1);
        }
    }
    #else
    void handleKeypress(cocos2d::enumKeyCodes key, bool down) override
    {
        bool enabled = false;
        if (auto mod = Mod::get()->getSubmodule("startpos-switcher"))
            enabled = mod->isEnabled();

        if (down && enabled) {
            if (auto pl = PlayLayer::get()) {
                auto spl = as<StartposPlayLayer*>(pl);

                if (key == enumKeyCodes::KEY_Q)
                    spl->setStartpos(spl->m_fields->selectedIndex - 1);

                else if (key == enumKeyCodes::KEY_E)
                    spl->setStartpos(spl->m_fields->selectedIndex + 1);
            }
        }

        UILayer::handleKeypress(key, down);
    }
    #endif
};

class StartposUIDelegate : public ModuleChangeDelegate
{
    virtual void initOptionsLayer(CCLayer* options) override
    {
    }
};

$execute
{
    Loader::get()->queueInMainThread([] {
        if (auto mod = Mod::get()->getSubmodule("startpos-switcher"))
            mod->delegate = new StartposUIDelegate();
    });
}
