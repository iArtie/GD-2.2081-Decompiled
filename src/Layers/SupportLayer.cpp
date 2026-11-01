#include "Layers/SupportLayer.h"
#include "Managers/GameManager.h"
#include "Managers/GJAccountManager.h"
#include "Managers/GameStatsManager.h"
#include "Utils/GameToolbox.h"
#include "Managers/AchievementManager.h"
#include "Sprites/ButtonSprite.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Layers/FLAlertLayer.h"
#include "Popups/UploadActionPopup.h"
#include "Layers/URLViewLayer.h"
#include "Cocos/CCURLObject.h"
#include "Layers/RewardUnlockLayer.h"
#include "Models/GJRewardItem.h"
#include "Utils/PlatformToolbox.h"
#include "Delegates/AppDelegate.h"

void SupportLayer::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (btn2)
        sendSupportMail();
}

SupportLayer* SupportLayer::create() {
    SupportLayer* returnVal = new SupportLayer();
    if (GJDropDownLayer::init("Support")) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

void SupportLayer::customSetup() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    CCMenuItemSpriteExtra* linksBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Links", 0, 1, 0.5f, true, "bigFont.fnt", "GJ_button_01.png", 30.0f),
        nullptr, menu_selector(SupportLayer::onLinks));
    CCMenuItemSpriteExtra* contactBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Contact", 0, 1, 0.5f, true, "bigFont.fnt", "GJ_button_01.png", 30.0f),
        nullptr, menu_selector(SupportLayer::onEmail));
    CCMenuItemSpriteExtra* privacyBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Privacy", 0, 1, 0.5f, true, "bigFont.fnt", "GJ_button_01.png", 30.0f),
        nullptr, menu_selector(SupportLayer::onPrivacy));
    CCMenuItemSpriteExtra* tosBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("ToS", 0, 1, 0.5f, true, "bigFont.fnt", "GJ_button_01.png", 30.0f),
        nullptr, menu_selector(SupportLayer::onTOS));
    CCMenuItemSpriteExtra* sfxBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("SFX", 0, 1, 0.5f, true, "bigFont.fnt", "GJ_button_01.png", 30.0f),
        nullptr, menu_selector(SupportLayer::onSFX));

    cocos2d::CCMenu* menu = cocos2d::CCMenu::create(linksBtn, contactBtn, privacyBtn, tosBtn, sfxBtn, nullptr);
    menu->setPosition(centerX, centerY + 60.0f);
    linksBtn->setPosition(-100.0f, 20.0f);
    contactBtn->setPosition(0.0f, 20.0f);
    sfxBtn->setPosition(100.0f, 20.0f);
    privacyBtn->setPosition(-50.0f, -20.0f);
    tosBtn->setPosition(50.0f, -20.0f);

    GameToolbox::createToggleButton(
        "Low Detail Mode",
        menu_selector(SupportLayer::onLowDetail),
        !GameManager::sharedState()->m_performanceMode,
        menu,
        cocos2d::CCPoint(centerX - 160.0f, centerY - 90.0f),
        this,
        m_mainLayer,
        0.8f, 0.6f, 0.0f, nullptr, "bigFont.fnt", false, 0, nullptr);

    GJAccountManager* accountManager = GJAccountManager::sharedState();
    if (accountManager->m_accountID - accountManager->m_unkInt1 > 0) {
        CCMenuItemSpriteExtra* reqBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("Req", 0, 0, 0.4f, true, "bigFont.fnt", "GJ_button_04.png", 25.0f),
            nullptr, menu_selector(SupportLayer::onRequestAccess));
        menu->addChild(reqBtn);
        reqBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(menu, &cocos2d::CCPoint(centerX + 150.0f, centerY - 90.0f)));

        if ((float)((float)rand() * 4.6566e-10f) > 0.9f) {
            if (!GameStatsManager::sharedState()->isSpecialChestUnlocked("0023")) {
                CCMenuItemSpriteExtra* rewardBtn = CCMenuItemSpriteExtra::create(
                    ButtonSprite::create("?", 0, 0, 0.6f, true, "goldFont.fnt", "GJ_button_05.png", 25.0f),
                    nullptr, menu_selector(SupportLayer::onGetReward));
                menu->addChild(rewardBtn);
                rewardBtn->setPosition(reqBtn->getPosition() + cocos2d::CCPoint(-50.0f, 0.0f));
            }
        }
    }

    m_mainLayer->addChild(menu, 1);

    float logoX = centerX - 55.0f;
    float logoX2 = centerX + 45.0f;
    cocos2d::CCMenu* logoMenu = cocos2d::CCMenu::create();

    cocos2d::CCSprite* developedBy = cocos2d::CCSprite::createWithSpriteFrameName("developedBy_001.png");
    m_mainLayer->addChild(developedBy);
    developedBy->setPosition(logoX, centerY);

    cocos2d::CCSprite* robtopLogo = cocos2d::CCSprite::createWithSpriteFrameName("robtoplogo_small.png");
    CCMenuItemSpriteExtra* robtopBtn = CCMenuItemSpriteExtra::create(robtopLogo, nullptr, menu_selector(SupportLayer::onRobTop));
    logoMenu->addChild(robtopBtn);
    robtopBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(logoMenu, &cocos2d::CCPoint(logoX2, developedBy->getPosition().y)));

    cocos2d::CCSprite* poweredBy = cocos2d::CCSprite::createWithSpriteFrameName("poweredBy_001.png");
    m_mainLayer->addChild(poweredBy);
    poweredBy->setPosition(logoX, developedBy->getPosition().y - 35.0f);

    cocos2d::CCSprite* cocosLogo = cocos2d::CCSprite::createWithSpriteFrameName("cocos2DxLogo.png");
    CCMenuItemSpriteExtra* cocosBtn = CCMenuItemSpriteExtra::create(cocosLogo, nullptr, menu_selector(SupportLayer::onCocos2d));
    logoMenu->addChild(cocosBtn);
    cocosBtn->setPosition(cocos2d::CCNode::convertToNodeSpace(logoMenu, &cocos2d::CCPoint(logoX2, poweredBy->getPosition().y)));

    cocos2d::CCLabelBMFont* versionLabel = cocos2d::CCLabelBMFont::create("v2.2", "bigFont.fnt");
    versionLabel->setScale(0.35f);
    m_mainLayer->addChild(versionLabel);
    versionLabel->setPosition(centerX, poweredBy->getPosition().y - 26.0f);

    if (!AchievementManager::sharedState()->isAchievementEarned("geometry.ach.secret08")) {
        cocos2d::CCLabelBMFont* secretLabel = cocos2d::CCLabelBMFont::create("secret", "bigFont.fnt");
        addChild(secretLabel, 3);
        secretLabel->setScale(0.5f);
        secretLabel->setColor(cocos2d::ccc3(255, 255, 255));
        secretLabel->setOpacity(0);
        secretLabel->setPosition(winSize.width - 32.0f, 8.0f);
        secretLabel->runAction(cocos2d::CCFadeTo::create(10.0f, 100));
    }

    m_mainLayer->addChild(logoMenu, 2);
}

void SupportLayer::exitLayer() {
    GJDropDownLayer::exitLayer(nullptr);
}

void SupportLayer::onClosePopup(UploadActionPopup* popup) {
    popup->closePopup();
    m_uploadPopup = nullptr;
}

void SupportLayer::onCocos2d(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.cocos2d-x.org/");
}

void SupportLayer::onEmail(cocos2d::CCObject* sender) {
    if (GameToolbox::doWeHaveInternet()) {
        FLAlertLayer::create(
            this,
            "Send Email",
            "Do you want to send an <cg>email</c> to <cy>RobTop Games</c> support?")->show();
    }
}

void SupportLayer::onGetReward(cocos2d::CCObject* sender) {
    GJRewardItem* reward = GameStatsManager::sharedState()->unlockSpecialChest("0023");
    if (reward) {
        RewardUnlockLayer* layer = RewardUnlockLayer::create(reward->m_rewardType, 0);
        addChild(layer, 100);
        layer->showCollectReward(reward);
    }
}

void SupportLayer::onLinks(cocos2d::CCObject* sender) {
    cocos2d::CCArray* links = cocos2d::CCArray::create();
    links->addObject(CCURLObject::create("FAQ", "https://www.robtopgames.com/faq"));
    links->addObject(CCURLObject::create("Editor Guide", "https://www.robtopgames.com/editor"));
    links->addObject(CCURLObject::create("Rating System", "https://www.robtopgames.com/rating"));
    links->addObject(CCURLObject::create("Leaderboards", "https://www.robtopgames.com/leaderboards"));
    links->addObject(CCURLObject::create("Editor Guide (Spanish)", "https://www.robtopgames.com/editor_es"));
    links->addObject(CCURLObject::create("Rating System (Spanish)", "https://www.robtopgames.com/rating_es"));
    links->addObject(CCURLObject::create("Leaderboards (Spanish)", "https://www.robtopgames.com/leaderboards_es"));
    links->addObject(CCURLObject::create("Editor Guide (Russian)", "https://www.robtopgames.com/editor_ru"));
    links->addObject(CCURLObject::create("Rating System (Russian)", "https://www.robtopgames.com/rating_ru"));
    links->addObject(CCURLObject::create("Leaderboards (Russian)", "https://www.robtopgames.com/leaderboards_ru"));

    URLViewLayer* layer = URLViewLayer::create("Support Links", links);
    addChild(layer, 100);
    layer->showLayer(false);
}

void SupportLayer::onLowDetail(cocos2d::CCObject* sender) {
    GameManager::sharedState()->unloadBackground();
    GameManager::sharedState()->m_performanceMode = !GameManager::sharedState()->m_performanceMode;
    if (!GameManager::sharedState()->m_showedLowDetailDialog) {
        GameManager::sharedState()->m_showedLowDetailDialog = true;
        FLAlertLayer::create(
            nullptr,
            "Low Detail Mode",
            "Low detail mode disables a lot of visual effects to increase performance.")->show();
    }
}

void SupportLayer::onPrivacy(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.robtopgames.com/privacy");
}

void SupportLayer::onRequestAccess(cocos2d::CCObject* sender) {
    GameLevelManager::sharedState()->m_uploadActionDelegate = this;
    if (GameLevelManager::sharedState()->requestUserAccess()) {
        m_uploadPopup = UploadActionPopup::create(this, "Loading...");
        m_uploadPopup->show();
    }
}

void SupportLayer::onRobTop(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.robtopgames.com/");
}

void SupportLayer::onSFX(cocos2d::CCObject* sender) {
    cocos2d::CCArray* links = cocos2d::CCArray::create();
    links->addObject(CCURLObject::create("Epic Stock Media", "https://www.epicstockmedia.com/"));
    links->addObject(CCURLObject::create("Cyberwave Orchestra", "https://www.cyberwaveorchestra.com/"));
    links->addObject(CCURLObject::create("Fusehive", "http://www.fusehive.com/"));
    links->addObject(CCURLObject::create("SoundMorph", "https://www.soundmorph.com/"));
    links->addObject(CCURLObject::create("Stormwave Audio", "https://www.stormwaveaudio.com/"));
    links->addObject(CCURLObject::create("David Dumais", "https://www.daviddumaisaudio.com/"));

    URLViewLayer* layer = URLViewLayer::create("SFX Library Credits", links);
    addChild(layer, 100);
    layer->showLayer(false);
}

void SupportLayer::onTOS(cocos2d::CCObject* sender) {
    cocos2d::CCApplication::sharedApplication()->openURL("https://www.robtopgames.com/tos");
}

void SupportLayer::sendSupportMail() {
    std::string udid = GameManager::sharedState()->m_playerUDID;
    std::string username = GJAccountManager::sharedState()->m_username;
    AppDelegate* app = AppDelegate::get();

    std::string body = cocos2d::CCString::createWithFormat(
        "\n\nGame: %s\n\nUserID: %s\n\nAccount: %s\n\nPlatform: %i\n\nVersion: %i - %i",
        "Geometry Dash", udid.c_str(), username.c_str(),
        app->m_platformID, 22, 48)->getCString();

    PlatformToolbox::sendMail("Geometry Dash Support", body.c_str(), "support@robtopgames.com");
}

void SupportLayer::uploadActionFailed(int id, int response) {
    if (response == 43 && m_uploadPopup) {
        m_uploadPopup->showFailMessage("Failed. Nothing found.");
    }
}

void SupportLayer::uploadActionFinished(int id, int response) {
    if (response == 43 && m_uploadPopup) {
        GameManager* gm = GameManager::sharedState();
        if (gm->m_hasRP.random - gm->m_hasRP.seed == 2) {
            m_uploadPopup->showSuccessMessage("Success! Elder Moderator\naccess granted.");
        } else if (gm->m_hasRP.random - gm->m_hasRP.seed == 1) {
            m_uploadPopup->showSuccessMessage("Success! Moderator access granted.");
        } else if (gm->m_hasDRP) {
            m_uploadPopup->showSuccessMessage("Success! Leaderboard Moderator\naccess granted.");
        }
    }
}

SupportLayer::~SupportLayer() {
}