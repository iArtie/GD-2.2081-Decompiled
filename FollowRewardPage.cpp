#include "FollowRewardPage.h"
#include "GameManager.h"
#include "GameStatsManager.h"
#include "GJChestSprite.h"
#include "CCMenuItemSpriteExtra.h"
#include "RewardUnlockLayer.h"
#include "DialogLayer.h"
#include "DialogObject.h"
#include "AdToolbox.h"
#include "MusicDownloadManager.h"
#include "FLAlertLayer.h"
#include "cocos2d.h"

void FollowRewardPage::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (!btn2) return;
    
    GameManager* gm = GameManager::sharedState();
    switch (layer->getTag()) {
        case 1: gm->subYouTube(); break;
        case 2: gm->followTwitter(); break;
        case 3: gm->likeFacebook(); break;
        case 4: gm->followTwitch(); break;
        case 5: gm->joinDiscord(); break;
        case 6: gm->joinReddit(); break;
        default: break;
    }
}

FollowRewardPage* FollowRewardPage::create() {
    FollowRewardPage* ret = new FollowRewardPage();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FollowRewardPage::init() {
    if (!FLAlertLayer::init(150)) {
        return false;
    }
    
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    
    // Background
    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setOpacity(255);
    bg->setContentSize(cocos2d::CCSize(420, 280));
    m_mainLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    
    // Corner decorations
    for (int i = 1; i <= 4; i++) {
        cocos2d::CCSprite* corner = cocos2d::CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        m_mainLayer->addChild(corner, 8);
        
        float xOffset = 185.0f;
        float yOffset = 115.0f;
        
        if (i == 2 || i == 3) xOffset = -185.0f;
        if (i == 3 || i == 4) yOffset = -115.0f;
        if (i == 2 || i == 4) corner->setFlipY(true);
        if (i == 3 || i == 4) corner->setFlipX(true);
        
        corner->setPosition(cocos2d::CCPoint(winSize.width * 0.5f + xOffset, winSize.height * 0.5f + yOffset));
    }
    
    // Title
    cocos2d::CCLabelBMFont* titleLabel = cocos2d::CCLabelBMFont::create();
    titleLabel->setScale(0.8f);
    m_mainLayer->addChild(titleLabel);
    titleLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 140.0f - 22.0f));
    
    // Subtitle
    cocos2d::CCLabelBMFont* subtitleLabel = cocos2d::CCLabelBMFont::create();
    m_mainLayer->addChild(subtitleLabel);
    subtitleLabel->setScale(0.5f);
    cocos2d::CCPoint titlePos = titleLabel->getPosition();
    subtitleLabel->setPosition(cocos2d::CCPoint(titlePos.x, titlePos.y - 26.0f));
    
    // Close button
    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(closeSprite, nullptr, this, menu_selector(FollowRewardPage::onClose));
    closeBtn->setSizeMult(1.6f);
    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_mainLayer->addChild(m_buttonMenu, 10);
    m_buttonMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 210.0f + 10.0f, winSize.height * 0.5f + 140.0f - 10.0f));
    
    m_chests = cocos2d::CCArray::create();
    m_chests->retain();
    
    cocos2d::CCPoint basePos = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 30.0f);
    
    // Create 6 chests for each social platform
    for (unsigned int i = 1; i <= 6; i++) {
        bool isUnlocked = false;
        bool ugv = false;
        std::string chestKey;
        std::string iconName;
        std::string platformName;
        std::string ugvKey;
        cocos2d::CCPoint offset;
        
        switch (i) {
            case 1: // YouTube
                chestKey = "0007";
                iconName = "gj_ytIcon_001.png";
                platformName = "YouTube";
                ugvKey = "22";
                offset = cocos2d::CCPoint(-90.0f, 48.0f);
                break;
            case 2: // Twitter
                chestKey = "0008";
                iconName = "gj_twIcon_001.png";
                platformName = "Twitter";
                ugvKey = "24";
                offset = cocos2d::CCPoint(0.0f, 48.0f);
                break;
            case 3: // Facebook
                chestKey = "0009";
                iconName = "gj_fbIcon_001.png";
                platformName = "Facebook";
                ugvKey = "23";
                offset = cocos2d::CCPoint(90.0f, 48.0f);
                break;
            case 4: // Twitch
                chestKey = "0010";
                iconName = "gj_twitchIcon_001.png";
                platformName = "Twitch";
                ugvKey = "26";
                offset = cocos2d::CCPoint(-90.0f, -48.0f);
                break;
            case 5: // Discord
                chestKey = "0011";
                iconName = "gj_discordIcon_001.png";
                platformName = "Discord";
                ugvKey = "27";
                offset = cocos2d::CCPoint(0.0f, -48.0f);
                break;
            case 6: // Reddit
                chestKey = "0024";
                iconName = "gj_rdIcon_001.png";
                platformName = "Reddit";
                ugvKey = "32";
                offset = cocos2d::CCPoint(90.0f, -48.0f);
                break;
        }
        
        GameStatsManager* gsm = GameStatsManager::sharedState();
        GJRewardItem* reward = gsm->getRewardForSpecialChest(chestKey);
        isUnlocked = gsm->isSpecialChestUnlocked(chestKey);
        
        GameManager* gm = GameManager::sharedState();
        ugv = gm->getUGV(ugvKey);
        
        GJChestSprite* chest = GJChestSprite::create(reward->m_rewardType);
        chest->switchToState(Closed, false);
        m_chests->addObject(chest);
        chest->setScale(0.5f);
        
        CCMenuItemSpriteExtra* chestBtn = CCMenuItemSpriteExtra::create(chest, nullptr, this, menu_selector(FollowRewardPage::onSpecialItem));
        chestBtn->setSelectSound("chestClick.ogg");
        chestBtn->setContentSize(cocos2d::CCSize(50, 40));
        chest->setPosition(cocos2d::CCPoint(chestBtn->getContentSize().width * 0.5f, chestBtn->getContentSize().height * 0.5f));
        m_buttonMenu->addChild(chestBtn);
        chestBtn->setAnimationEnabled(false);
        chestBtn->setColorEnabled(true);
        chestBtn->setTag(i);
        chest->setTag(i);
        cocos2d::CCPoint pos = basePos + offset;
        chestBtn->setPosition(m_buttonMenu->convertToNodeSpace(pos));
        
        // Platform name label
        cocos2d::CCLabelBMFont* nameLabel = cocos2d::CCLabelBMFont::create();
        nameLabel->setScale(0.5f);
        m_mainLayer->addChild(nameLabel);
        cocos2d::CCPoint labelPos = pos + cocos2d::CCPoint(0.0f, 42.0f);
        nameLabel->setPosition(labelPos);
        
        if (isUnlocked) {
            switchToOpenedState(chestBtn);
        } else if (!ugv) {
            // Show platform icon overlay
            cocos2d::CCSprite* icon = cocos2d::CCSprite::createWithSpriteFrameName(iconName.c_str());
            icon->setScale(1.2f);
            icon->setTag(i + 10);
            m_mainLayer->addChild(icon, 11);
            icon->setPosition(pos + cocos2d::CCPoint(0.0f, 2.0f));
            
            chest->m_dark = true;
            chest->setColor(cocos2d::ccColor3B{150, 150, 150});
        }
    }
    
    GameManager::sharedState()->m_gameRateDelegate2 = this;
    return true;
}

void FollowRewardPage::keyBackClicked() {
    onClose(nullptr);
}

void FollowRewardPage::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void FollowRewardPage::onRewardedVideo(cocos2d::CCObject* sender) {
    if (!m_videoPlaying && AdToolbox::hasCachedRewardedVideo()) {
        if (GameManager::sharedState()->canShowRewardedVideo()) {
            m_videoPlaying = true;
            GameManager::sharedState()->m_rewardedVideoDelegate = this;
            AdToolbox::showRewardedVideo();
        } else {
            // Show dialog about no chests
            GameStatsManager::sharedState()->getNextVideoAdReward();
            // Show dialog sequence
            cocos2d::CCArray* dialogs = cocos2d::CCArray::create();
            dialogs->addObject(DialogObject::create("Scratch", "I don't have any <co>chests</c> left<d010>.<d010>.<d010>.", 12, 1.0f, false, cocos2d::ccColor3B{255, 255, 255}));
            dialogs->addObject(DialogObject::create("Scratch", "You took <cr>EVERYTHING</c> I had!", 14, 1.0f, false, cocos2d::ccColor3B{255, 255, 255}));
            dialogs->addObject(DialogObject::create("Scratch", "I need to find <cy>new stuff</c>...", 13, 1.0f, false, cocos2d::ccColor3B{255, 255, 255}));
            
            DialogLayer* dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
            m_mainLayer->addChild(dialogLayer, 100);
            dialogLayer->updateChatPlacement(Center);
            dialogLayer->animateInRandomSide();
        }
    }
}

void FollowRewardPage::onSpecialItem(cocos2d::CCObject* sender) {
    unsigned int tag = sender->getTag();
    bool ugv = false;
    std::string chestKey;
    std::string platformName;
    std::string description;
    std::string buttonText;
    
    switch (tag) {
        case 1: // YouTube
            chestKey = "0007";
            platformName = "YouTube";
            description = "Subscribe to <cg>RobTop Games</c> on <cr>YouTube</c>\nto unlock this <cy>chest</c>!";
            buttonText = "Sub!";
            ugv = GameManager::sharedState()->getUGV("22");
            break;
        case 2: // Twitter
            chestKey = "0008";
            platformName = "Twitter";
            description = "Follow <cg>RobTop Games</c> on <cl>Twitter</c>\nto unlock this <cy>chest</c>!";
            buttonText = "Follow!";
            ugv = GameManager::sharedState()->getUGV("24");
            break;
        case 3: // Facebook
            chestKey = "0009";
            platformName = "Facebook";
            description = "Like <cg>Geometry Dash</c> on <cr>Facebook</c>\nto unlock this <cy>chest</c>!";
            buttonText = "Like!";
            ugv = GameManager::sharedState()->getUGV("23");
            break;
        case 4: // Twitch
            chestKey = "0010";
            platformName = "Twitch";
            description = "Follow <cg>Geometry Dash</c> on <cr>Twitch</c>\nto unlock this <cy>chest</c>!";
            buttonText = "Follow!";
            ugv = GameManager::sharedState()->getUGV("26");
            break;
        case 5: // Discord
            chestKey = "0011";
            platformName = "Discord";
            description = "Join <cg>Geometry Dash</c> on <cr>Discord</c>\nto unlock this <cy>chest</c>!";
            buttonText = "Join!";
            ugv = GameManager::sharedState()->getUGV("27");
            break;
        case 6: // Reddit
            chestKey = "0024";
            platformName = "Reddit";
            description = "Join <cg>Geometry Dash</c> on <cr>Reddit</c>\nto unlock this <cy>chest</c>!";
            buttonText = "Join!";
            ugv = GameManager::sharedState()->getUGV("32");
            break;
        default:
            return;
    }
    
    if (ugv) {
        GJRewardItem* reward = GameStatsManager::sharedState()->unlockSpecialChest(chestKey);
        if (reward) {
            switchToOpenedState((CCMenuItemSpriteExtra*)sender);
            RewardUnlockLayer* rewardLayer = RewardUnlockLayer::create(reward->m_rewardType, false);
            addChild(rewardLayer, 100);
            rewardLayer->showCollectReward(reward);
        }
    } else {
        FLAlertLayer* alert = FLAlertLayer::create(this, platformName, description);
        alert->setTag(tag);
        alert->m_button2->updateBGImage("GJ_button_02.png");
        alert->show();
    }
}

void FollowRewardPage::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void FollowRewardPage::rewardedVideoFinished() {
    m_videoPlaying = false;
}

void FollowRewardPage::show() {
    if (!m_noElasticity) {
        unsigned char opacity = getOpacity();
        cocos2d::CCPoint pos = m_mainLayer->getPosition();
        
        m_mainLayer->setPosition(cocos2d::CCPoint(-160.0f, pos.y));
        cocos2d::CCMoveTo* move = cocos2d::CCMoveTo::create(0.5f, pos);
        cocos2d::CCEaseElasticOut* ease = cocos2d::CCEaseElasticOut::create(move, 0.6f);
        m_mainLayer->runAction(ease);
        
        cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
        cocos2d::CCScene* scene = director->m_pRunningScene;
        
        if (!m_scene) {
            m_scene = scene;
            int highestZ = scene->getHighestChildZ();
            m_ZOrder = (highestZ > 104) ? highestZ + 1 : 105;
        }
        
        if (m_ZOrder == 0) {
            m_ZOrder = 105;
        }
        
        scene->addChild(this, m_ZOrder);
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
        setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

void FollowRewardPage::switchToOpenedState(CCMenuItemSpriteExtra* button) {
    GJChestSprite* chest = (GJChestSprite*)button->getNormalImage();
    chest->m_dark = false;
    chest->switchToState(Opened, true);
    
    // Update all child sprites to be brighter
    cocos2d::CCArray* children = chest->getChildren();
    if (children) {
        for (unsigned int i = 0; i < children->count(); i++) {
            cocos2d::CCSprite* child = (cocos2d::CCSprite*)children->objectAtIndex(i);
            if (child) {
                child->setOpacity(0xFFFFFFFF);
                child->setColor(cocos2d::ccColor3B{150, 150, 150});
            }
        }
    }
    
    button->setEnabled(false);
}

void FollowRewardPage::updateRate() {
    for (unsigned int i = 0; i < m_chests->count(); i++) {
        bool isUnlocked = false;
        bool ugv = false;
        std::string chestKey;
        std::string ugvKey;
        
        switch (i) {
            case 0: chestKey = "0007"; ugvKey = "22"; break;
            case 1: chestKey = "0008"; ugvKey = "24"; break;
            case 2: chestKey = "0009"; ugvKey = "23"; break;
            case 3: chestKey = "0010"; ugvKey = "26"; break;
            case 4: chestKey = "0011"; ugvKey = "27"; break;
            case 5: chestKey = "0024"; ugvKey = "32"; break;
            default: continue;
        }
        
        isUnlocked = GameStatsManager::sharedState()->isSpecialChestUnlocked(chestKey);
        ugv = GameManager::sharedState()->getUGV(ugvKey);
        
        if (ugv && !isUnlocked) {
            cocos2d::CCObject* obj = m_chests->objectAtIndex(i);
            obj->setColor(cocos2d::ccColor3B{150, 150, 150});
            
            int tag = obj->getTag();
            m_mainLayer->removeChildByTag(tag + 10);
        }
    }
}

FollowRewardPage::~FollowRewardPage() {
    if (m_chests) {
        m_chests->release();
        m_chests = nullptr;
    }
}