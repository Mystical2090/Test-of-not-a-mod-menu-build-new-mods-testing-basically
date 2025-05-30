#pragma once

#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class MyPopup : public geode::Popup<std::string const&> {
protected:
    CCLabelBMFont *jumphacklabel = nullptr, *nocliplabel = nullptr, *ignoreinputslabel = nullptr, *autoclickerlabel = nullptr,
                 *instantrespawnlabel = nullptr, *layoutmodelabel = nullptr, *practicemusichacklabel = nullptr,
                 *iconhacklabel = nullptr, *colorhacklabel = nullptr, *speedhacklabel = nullptr, *showtrajectorylabel = nullptr,
                 *rgbiconslabel = nullptr;
    CCMenuItemToggler* noclipcheckbox = nullptr;
    CCSprite *rightplayerarrowbtn = nullptr, *leftplayerarrowbtn = nullptr, *rightmiscarrowbtn = nullptr, *leftmiscarrowbtn = nullptr;
    CCLayer* m_mainLayer = nullptr;

    bool setup(std::string const& value) override {
        this->setTitle("Player");
        this->m_title->setScale(1.2f);

        auto popupSize = this->getContentSize();
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        log::info("Setting up main layer");
        m_mainLayer = CCLayer::create();
        if (!m_mainLayer) {
            log::error("Failed to create m_mainLayer");
            return false;
        }
        this->addChild(m_mainLayer);

        auto label = CCLabelBMFont::create("Polo v1.0.0", "goldFont.fnt");
        label->setScale(0.3f);
        label->setAnchorPoint({1.0f, 0.0f});
        label->setPosition({ popupSize.width - 10.f, 10.f });

        jumphacklabel = createLabel("Jump Hack", { popupSize.width / 2 - 70.f, popupSize.height / 2 });
        nocliplabel = createLabel("Noclip", { jumphacklabel->getPositionX(), jumphacklabel->getPositionY() + 40.f });
        ignoreinputslabel = createLabel("Ignore Inputs", { jumphacklabel->getPositionX(), jumphacklabel->getPositionY() - 40.f });
        autoclickerlabel = createLabel("AutoClicker", { jumphacklabel->getPositionX(), jumphacklabel->getPositionY() - 80.f });

        layoutmodelabel = createLabel("Layout Mode", { popupSize.width / 2 - 70.f, popupSize.height / 2 - 10.f });
        practicemusichacklabel = createLabel("Practice Music Hack", { layoutmodelabel->getPositionX(), layoutmodelabel->getPositionY() + 40.f });
        iconhacklabel = createLabel("Icon Hack", { layoutmodelabel->getPositionX(), layoutmodelabel->getPositionY() - 50.f });
        colorhacklabel = createLabel("Color Hack", { layoutmodelabel->getPositionX(), layoutmodelabel->getPositionY() - 100.f });
        speedhacklabel = createLabel("Speed Hack", { layoutmodelabel->getPositionX(), layoutmodelabel->getPositionY() - 150.f });
        instantrespawnlabel = createLabel("Instant Respawn", { layoutmodelabel->getPositionX(), layoutmodelabel->getPositionY() - 200.f });
        showtrajectorylabel = createLabel("Show Trajectory", { layoutmodelabel->getPositionX(), layoutmodelabel->getPositionY() + 80.f });
        rgbiconslabel = createLabel("Rgb Icons", { layoutmodelabel->getPositionX(), layoutmodelabel->getPositionY() + 120.f });

        bool noclipEnabled = Mod::get()->getSavedValue<bool>("enable-noclip", false);
        log::info("Loaded noclip state: {}", noclipEnabled);

        auto checkboxOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto checkboxOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        if (!checkboxOff || !checkboxOn) {
            log::error("Failed to load checkbox sprites");
            return false;
        }

        noclipcheckbox = CCMenuItemToggler::create(checkboxOff, checkboxOn, this, menu_selector(MyPopup::onNoclipCheckbox));
        noclipcheckbox->setPosition(jumphacklabel->getPositionX() + 95.f, jumphacklabel->getPositionY() + 60.f);
        noclipcheckbox->setScale(0.9f);
        noclipcheckbox->toggle(noclipEnabled);

        auto menu = CCMenu::create();
        menu->addChild(noclipcheckbox);
        menu->addChild(label);
        menu->setPosition({0, 0});

        this->addChild(menu);

        toggleMiscVisibility(false);

        return true;
    }

    CCLabelBMFont* createLabel(const std::string& text, const CCPoint& pos) {
        auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
        if (!label) {
            log::error("Failed to create label: {}", text);
            return nullptr;
        }
        label->setScale(0.5f);
        label->setColor({255, 255, 255});
        label->setZOrder(1);
        label->setPosition(pos);
        label->setContentSize({ 100.f, label->getContentSize().height });
        m_mainLayer->addChild(label);
        return label;
    }

    void toggleMiscVisibility(bool visible) {
        if (layoutmodelabel) layoutmodelabel->setVisible(visible);
        if (practicemusichacklabel) practicemusichacklabel->setVisible(visible);
        if (instantrespawnlabel) instantrespawnlabel->setVisible(visible);
        if (speedhacklabel) speedhacklabel->setVisible(visible);
        if (iconhacklabel) iconhacklabel->setVisible(visible);
        if (colorhacklabel) colorhacklabel->setVisible(visible);
        if (rgbiconslabel) rgbiconslabel->setVisible(visible);
        if (showtrajectorylabel) showtrajectorylabel->setVisible(visible);
    }

    void onNoclipCheckbox(cocos2d::CCObject* sender) {
        auto checkbox = static_cast<CCMenuItemToggler*>(sender);
        if (!checkbox) {
            log::error("onNoclipCheckbox called with invalid sender");
            return;
        }
        bool toggled = checkbox->isToggled();
        Mod::get()->setSavedValue("enable-noclip", toggled);
        log::info("Toggled noclip: {}", toggled ? "ON" : "OFF");
    }

public:
    static MyPopup* create(std::string const& text) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        float popupWidth = winSize.width * 0.9f;
        float popupHeight = winSize.height * 0.8f;

        auto ret = new MyPopup();
        if (ret->initAnchored(popupWidth, popupHeight, text)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};
