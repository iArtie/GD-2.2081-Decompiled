#include "SecretRewardsLayer.h"
#include "BoomScrollLayer.h"
#include "GJChestSprite.h"
#include "GJRewardItem.h"
#include "RewardUnlockLayer.h"
#include "GameStatsManager.h"
#include "GameManager.h"
#include "GameToolbox.h"
#include "PlatformToolbox.h"
#include "DialogLayer.h"
#include "DialogObject.h"
#include "FMODAudioEngine.h"
#include "CreatorLayer.h"
#include "GJShopLayer.h"
#include "DynamicScrollDelegate.h"

SecretRewardsLayer* SecretRewardsLayer::create(bool fromShop) {
    SecretRewardsLayer* layer = new SecretRewardsLayer();
    if (layer && layer->init(fromShop)) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

cocos2d::CCScene* SecretRewardsLayer::scene(bool fromShop) {
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    SecretRewardsLayer* layer = SecretRewardsLayer::create(fromShop);
    scene->addChild(layer);
    return scene;
}

void SecretRewardsLayer::createSecondaryLayer(int chestType) {
    if (m_secondaryLayer) {
        m_secondaryScrollLayer = nullptr;
        m_chestCounter = nullptr;
        m_secondaryLayer->removeFromParent();
        m_secondaryLayer = nullptr;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_secondaryLayer = cocos2d::CCLayer::create();
    addChild(m_secondaryLayer);

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create(
        "GJ_square05.png", cocos2d::CCRect(0, 0, 80, 80));
    bg->setOpacity(75);
    bg->setContentSize(cocos2d::CCSize(370, 255));
    m_secondaryLayer->addChild(bg, -1);
    bg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    bg->setColor(m_backgroundSprite->getColor());

    for (int i = 0; i < 2; i++) {
        cocos2d::CCSprite* chain = cocos2d::CCSprite::createWithSpriteFrameName("chain_01_001.png");
        m_secondaryLayer->addChild(chain, -1);
        chain->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.0f));
        chain->setPosition(cocos2d::CCPoint(
            winSize.width * 0.5f - 100.0f + (float)(i * 100.0f * 2.0f),
            winSize.height * 0.5f - 2.0f));
        chain->setColor(ccc3(50, 50, 50));
        chain->setOpacity(75);
    }

    m_chestCounter = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_secondaryLayer->addChild(m_chestCounter);
    m_chestCounter->setScale(0.6f);
    m_chestCounter->setPosition(cocos2d::CCPoint(
        winSize.width * 0.5f, winSize.height * 0.5f + 17.0f));

    updateUnlockedLabel();

    cocos2d::CCArray* chestItems = generateChestItems(chestType);
    m_secondaryScrollLayer = BoomScrollLayer::create(chestItems, 0, chestType <= 5, 0, 0, nullptr);
    m_secondaryLayer->addChild(m_secondaryScrollLayer);

    float screenBottom = cocos2d::CCDirector::sharedDirector()->getScreenBottom();
    m_secondaryScrollLayer->setPagesIndicatorPosition(cocos2d::CCPoint(winSize.width * 0.5f, screenBottom + 10.0f));
    m_secondaryScrollLayer->m_doVisit = true;
    m_secondaryScrollLayer->m_unkFloat2 = 0.9f;
    m_secondaryScrollLayer->m_clippingRect = cocos2d::CCRect(0, 0,
        cocos2d::CCDirector::sharedDirector()->getScreenRight(),
        cocos2d::CCDirector::sharedDirector()->getScreenTop());
}

void SecretRewardsLayer::dialogClosed(DialogLayer* layer) {
    if (layer->getTag() == 2) {
        GameManager::sharedState()->setUGV("20", true);
        showShop(2);
    } else if (layer->getTag() == 1) {
        GameManager::sharedState()->setUGV("11", true);
        showShop(1);
    } else if (layer->getTag() == 3) {
        GameManager::sharedState()->setUGV("34", true);
        showShop(4);
    } else if (layer->getTag() == 4) {
        GameManager::sharedState()->setUGV("35", true);
        showShop(3);
    }
}

void SecretRewardsLayer::fadeInMusic() {
    FMODAudioEngine::sharedEngine()->fadeMusic(2.0f, 0, 0.2f, 1.0f);
}

void SecretRewardsLayer::fadeInOutMusic() {
    FMODAudioEngine::sharedEngine()->fadeMusic(1.0f, 0, 1.0f, 0.2f);
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(3.2f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretRewardsLayer::fadeInMusic)),
        nullptr);
    cocos2d::CCNode::runAction(this, seq);
}

cocos2d::CCArray* SecretRewardsLayer::generateChestItems(int chestType) {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    cocos2d::CCPoint center = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 80.0f);

    cocos2d::CCDictionary* treasureRoomChests = GameStatsManager::sharedState()->m_treasureRoomChests;

    std::vector<int> unlockedChestIDs;
    std::map<int, int> chestMapping;

    int startIdx = 0;
    int endIdx = 0;
    if (chestType >= 3 && chestType <= 8) {
        int idx = chestType - 3;
        startIdx = 0;
        endIdx = 0;
    }

    cocos2d::CCArray* pages = cocos2d::CCArray::create();
    cocos2d::CCArray* menus = cocos2d::CCArray::create();

    for (int i = 0; i < 12; i++) {
        int pageIdx = i / 12;
        if ((unsigned int)pages->count() <= (unsigned int)pageIdx) {
            cocos2d::CCLayer* pageLayer = cocos2d::CCLayer::create();
            cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
            pageLayer->addChild(menu);
            pages->addObject(pageLayer);
            menus->addObject(menu);
        } else {
            menus->objectAtIndex(pageIdx);
        }
    }

    return pages;
}

cocos2d::ccColor3B SecretRewardsLayer::getPageColor(int page) {
    switch (page) {
    case 1: return ccc3(50, 50, 50);
    case 2: return ccc3(70, 0, 120);
    case 3: return ccc3(36, 27, 40);
    default: return ccc3(70, 0, 75);
    }
}

void SecretRewardsLayer::goToPage(int page) {
    if (!m_inMainLayer) {
        m_mainScrollLayer = m_secondaryScrollLayer;
    }
    if (m_mainScrollLayer) {
        m_mainScrollLayer->getTotalPages();
        m_mainScrollLayer->moveToPage(page);
    }
}

bool SecretRewardsLayer::init(bool fromShop) {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }

    m_inMainLayer = true;

    cocos2d::CCTextureCache::sharedTextureCache()->addImage("TreasureRoomSheet.png", false);
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("TreasureRoomSheet.plist");

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    if (GameManager::sharedState()->getUGV("10")) {
        m_scratchDialogIndex = 0;
    }
    if (GameManager::sharedState()->getUGV("21")) {
        m_potborDialogIndex = 3;
    }

    GameManager::sharedState()->fadeInMusic("secretLoop.mp3");

    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer, 0);

    m_backgroundSprite = cocos2d::CCSprite::create("GJ_gradientBG.png");
    m_backgroundSprite->setAnchorPoint(cocos2d::CCPoint(0, 0));
    addChild(m_backgroundSprite, -3);
    m_backgroundSprite->setScaleX((winSize.width + 10.0f) / m_backgroundSprite->getContentSize().width);
    m_backgroundSprite->setScaleY((winSize.height + 10.0f) / m_backgroundSprite->getContentSize().height);
    m_backgroundSprite->setPosition(cocos2d::CCPoint(-5, -5));
    m_backgroundSprite->setColor(ccc3(70, 0, 75));

    cocos2d::CCSprite* web1 = cocos2d::CCSprite::createWithSpriteFrameName("treasureRoomSpiderweb_001.png");
    web1->setAnchorPoint(cocos2d::CCPoint(1, 1));
    web1->setPosition(cocos2d::CCPoint(winSize.width + 5.0f, winSize.height + 5.0f));
    web1->setOpacity(63);
    web1->setColor(ccc3(0, 0, 0));
    addChild(web1, -1);
    web1->setScale(1.1f);

    cocos2d::CCSprite* web2 = cocos2d::CCSprite::createWithSpriteFrameName("treasureRoomSpiderweb_001.png");
    web2->setAnchorPoint(cocos2d::CCPoint(0, 1));
    web2->setPosition(cocos2d::CCPoint(-5, winSize.height + 5.0f));
    web2->setOpacity(63);
    web2->setFlipX(true);
    web2->setColor(ccc3(0, 0, 0));
    addChild(web2, -1);
    web2->setScale(1.1f);

    m_backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    CCMenuItemSpriteExtra* backButton = CCMenuItemSpriteExtra::create(
        m_backSprite, nullptr, menu_selector(SecretRewardsLayer::onBack));
    backButton->setSizeMult(1.6f);
    cocos2d::CCMenu* backMenu = cocos2d::CCMenu::create(backButton, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f));

    if (PlatformToolbox::isControllerConnected()) {
        GameToolbox::addBackButton(this, backButton);
    }

    int keyStat = GameStatsManager::sharedState()->getStat("21");
    m_keysLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("%i", keyStat)->getCString(), "bigFont.fnt");
    m_keysLabel->setPosition(cocos2d::CCPoint(winSize.width - 55.0f, winSize.height - 18.0f));
    m_keysLabel->setAnchorPoint(cocos2d::CCPoint(1, 0.5f));
    m_keysLabel->setScale(0.6f);
    addChild(m_keysLabel);

    m_keySprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_bigKey_001.png");
    addChild(m_keySprite, 3);
    m_keySprite->setPosition(m_keysLabel->getPosition() + cocos2d::CCPoint(25.0f, -0.5f));

    int goldKeyStat = GameStatsManager::sharedState()->getStat("43");
    m_goldKeysLabel = cocos2d::CCLabelBMFont::create(
        cocos2d::CCString::createWithFormat("%i", goldKeyStat)->getCString(), "bigFont.fnt");
    m_goldKeysLabel->setPosition(m_keysLabel->getPosition());
    m_goldKeysLabel->setAnchorPoint(cocos2d::CCPoint(1, 0.5f));
    m_goldKeysLabel->setScale(0.6f);
    addChild(m_goldKeysLabel);

    m_goldKeySprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_bigGoldKey_001.png");
    addChild(m_goldKeySprite, 3);
    m_goldKeySprite->setPosition(m_goldKeysLabel->getPosition() + cocos2d::CCPoint(25.0f, -0.5f));

    m_goldKeysLabel->setVisible(false);
    m_goldKeySprite->setVisible(false);

    cocos2d::CCSprite* labelSprite = cocos2d::CCSprite::createWithSpriteFrameName("treasureRoomLabel_001.png");
    m_mainLayer->addChild(labelSprite, 2);
    labelSprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height - 42.0f));

    cocos2d::CCSprite* ground = cocos2d::CCSprite::createWithSpriteFrameName("treasureRoomGround_001.png");
    addChild(ground, 2);
    ground->setAnchorPoint(cocos2d::CCPoint(0, 1));
    ground->setPosition(cocos2d::CCPoint(-10.0f, 35.0f));
    ground->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE});
    ground->setScale((winSize.width + 20.0f) / ground->getContentSize().width);
    ground->setOpacity(150);

    cocos2d::CCArray* pageLayers = cocos2d::CCArray::create();
    cocos2d::CCArray* pageMenus = cocos2d::CCArray::create();
    m_chestCounters = cocos2d::CCDictionary::create();
    m_chestCounters->retain();

    for (int i = 0; i < 9; i++) {
        float row = floorf((float)i / 3.0f);
        int rowIdx = (int)row;
        if ((unsigned int)pageLayers->count() < (unsigned int)(rowIdx + 1)) {
            cocos2d::CCLayer* layer = cocos2d::CCLayer::create();
            cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
            layer->addChild(menu);
            pageLayers->addObject(layer);
            pageMenus->addObject(menu);
        }

        cocos2d::CCNode* pageNode = (cocos2d::CCNode*)pageLayers->objectAtIndex(rowIdx);
        int col = i % 3;
        cocos2d::CCNode* menuNode = (cocos2d::CCNode*)pageMenus->objectAtIndex(rowIdx);

        cocos2d::CCPoint chestPos;
        GJRewardType rewardType;
        if (col != 0) {
            chestPos = cocos2d::CCPoint(
                (float)(col - 1) * 100.0f + winSize.width * 0.5f,
                winSize.height * 0.5f + 10.0f);
            rewardType = (i >= 6) ? GJRewardType::Large : (GJRewardType)(i + 3);
        } else {
            if (i <= 5) {
                rewardType = (GJRewardType)(i + 3);
                std::string labelName = cocos2d::CCString::createWithFormat(
                    "tier%ilabel_001.png", (int)(row + 1.0f))->getCString();
                cocos2d::CCSprite* tierLabel = cocos2d::CCSprite::createWithSpriteFrameName(labelName.c_str());
                pageNode->addChild(tierLabel, 2);
                tierLabel->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 70.0f));
                tierLabel->setScale(0.8f);
            } else {
                rewardType = GJRewardType::Large;
            }
            chestPos = cocos2d::CCPoint(winSize.width * 0.5f - 100.0f, winSize.height * 0.5f + 10.0f);
        }

        std::string priceFrame = cocos2d::CCString::createWithFormat(
            "chest_0%i_price_001.png", (int)rewardType)->getCString();
        std::string chestFrame = cocos2d::CCString::createWithFormat(
            "chest_0%i_02_001.png", (int)rewardType)->getCString();

        bool isUnlocked = false;
        float priceScale = 1.0f;
        ccColor3B platformColor = ccc3(100, 100, 100);

        if (i <= 2) {
            platformColor = ccc3(150, 130, 80);
            priceScale = 0.8f;
        } else if (i <= 5) {
            priceScale = 0.8f;
        } else {
            platformColor = ccc3(100, 0, 180);
            int specialIdx = col + 1;
            priceFrame = cocos2d::CCString::createWithFormat(
                "chestSpecial_0%i_price_001.png", specialIdx)->getCString();
            if (specialIdx == 2) {
                isUnlocked = GameStatsManager::isSpecialChestUnlocked(
                    GameStatsManager::sharedState(), "0005");
            } else if (specialIdx == 3) {
                isUnlocked = GameStatsManager::isSpecialChestUnlocked(
                    GameStatsManager::sharedState(), "0006");
            } else {
                isUnlocked = GameStatsManager::isSpecialChestUnlocked(
                    GameStatsManager::sharedState(), "0004");
            }
            priceScale = 1.0f;
        }

        GJChestSprite* chest = GJChestSprite::create(rewardType);
        chest->setScale(0.6f);
        CCMenuItemSpriteExtra* chestBtn = CCMenuItemSpriteExtra::create(
            chest, nullptr, menu_selector(SecretRewardsLayer::onChestType));
        chestBtn->m_selectSound = "chestClick.ogg";
        chestBtn->setContentSize(cocos2d::CCSize(60, 48));
        chest->setPosition(cocos2d::CCPoint(
            chestBtn->getContentSize().width * 0.5f,
            chestBtn->getContentSize().height * 0.5f));
        menuNode->addChild(chestBtn);
        chestBtn->m_colorEnabled = true;
        chestBtn->m_animationEnabled = false;
        cocos2d::CCPoint nodePos = cocos2d::CCNode::convertToNodeSpace(menuNode, chestPos);
        chestBtn->setPosition(nodePos);
        chestBtn->setTag(i);

        if (isUnlocked) {
            switchToOpenedState(chestBtn);
        }

        cocos2d::CCSprite* platform = cocos2d::CCSprite::createWithSpriteFrameName("chestPlatform_01_001.png");
        pageNode->addChild(platform, 2);
        platform->setPosition(chestPos + cocos2d::CCPoint(0, -34.0f));
        platform->setColor(platformColor);
        platform->setOpacity(150);

        cocos2d::CCSprite* priceSprite = cocos2d::CCSprite::createWithSpriteFrameName(priceFrame.c_str());
        pageNode->addChild(priceSprite, 2);
        priceSprite->setPosition(platform->getPosition() + cocos2d::CCPoint(0, -20.0f));
        priceSprite->setScale(priceScale);

        if (i <= 5) {
            int unlockedCount = GameStatsManager::countUnlockedSecretChests(
                GameStatsManager::sharedState(), rewardType);
            int totalCount = GameStatsManager::countSecretChests(
                GameStatsManager::sharedState(), rewardType);
            cocos2d::CCLabelBMFont* counter = cocos2d::CCLabelBMFont::create(
                cocos2d::CCString::createWithFormat("%i / %i", unlockedCount, totalCount)->getCString(),
                "bigFont.fnt");
            pageNode->addChild(counter);
            counter->setScale(0.4f);
            counter->setPosition(priceSprite->getPosition() + cocos2d::CCPoint(0, -35.0f));
            counter->setColor(unlockedCount < totalCount ? ccc3(255, 200, 100) : ccc3(0, 255, 0));
            m_chestCounters->setObject(counter,
                cocos2d::CCString::createWithFormat("%i", (int)rewardType)->getCString());
        }

        if (i == 6) {
            cocos2d::CCPoint labelPos = cocos2d::CCPoint(
                winSize.width * 0.5f + 10.0f, winSize.height * 0.5f + 72.0f);
            m_chestLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
            m_chestLabel->setPosition(labelPos);
            m_chestLabel->setAnchorPoint(cocos2d::CCPoint(1, 0.5f));
            m_chestLabel->setScale(0.5f);
            pageNode->addChild(m_chestLabel);

            cocos2d::CCSprite* smallChest = cocos2d::CCSprite::createWithSpriteFrameName("chest03_small_001.png");
            pageNode->addChild(smallChest);
            smallChest->setPosition(labelPos + cocos2d::CCPoint(22.0f, 0.0f));
            updateUnlockedLabel();
        }
    }

    cocos2d::CCPoint goldChestPos = cocos2d::CCPoint(
        winSize.width * 0.5f, winSize.height * 0.5f + 12.0f);
    cocos2d::CCLayer* goldLayer = cocos2d::CCLayer::create();
    pageLayers->addObject(goldLayer);
    cocos2d::CCMenu* goldMenu = cocos2d::CCMenu::create();
    goldLayer->addChild(goldMenu);

    GJChestSprite* goldChest = GJChestSprite::create((GJRewardType)9);
    goldChest->setScale(0.72f);
    CCMenuItemSpriteExtra* goldBtn = CCMenuItemSpriteExtra::create(
        goldChest, nullptr, menu_selector(SecretRewardsLayer::onGoldChest));
    goldBtn->m_selectSound = "chestClick.ogg";
    goldBtn->setContentSize(cocos2d::CCSize(
        goldChest->getScale() * 100.0f, goldChest->getScale() * 80.0f));
    goldChest->setPosition(cocos2d::CCPoint(
        goldBtn->getContentSize().width * 0.5f,
        goldBtn->getContentSize().height * 0.5f));
    goldMenu->addChild(goldBtn);
    goldBtn->m_colorEnabled = true;
    goldBtn->m_animationEnabled = false;
    cocos2d::CCPoint goldNodePos = cocos2d::CCNode::convertToNodeSpace(goldMenu, goldChestPos);
    goldBtn->setPosition(goldNodePos);

    if (!GameStatsManager::getNextGoldChestID(GameStatsManager::sharedState())) {
        switchToOpenedState(goldBtn);
    }

    cocos2d::CCSprite* goldPlatform = cocos2d::CCSprite::createWithSpriteFrameName("chestPlatform_01_001.png");
    goldLayer->addChild(goldPlatform, 2);
    goldPlatform->setPosition(goldChestPos + cocos2d::CCPoint(0, -40.8f));
    goldPlatform->setColor(ccc3(100, 70, 60));
    goldPlatform->setOpacity(150);
    goldPlatform->setScale(1.2f);

    cocos2d::CCSprite* goldPrice = cocos2d::CCSprite::createWithSpriteFrameName("chest_09_price_001.png");
    goldLayer->addChild(goldPrice, 2);
    goldPrice->setPosition(goldPlatform->getPosition() + cocos2d::CCPoint(0, -24.0f));
    goldPrice->setScale(0.96f);

    cocos2d::CCPoint goldLabelPos = cocos2d::CCPoint(
        winSize.width * 0.5f, winSize.height * 0.5f + 72.0f);
    m_goldPriceSprite = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_goldPriceSprite->setPosition(goldLabelPos);
    m_goldPriceSprite->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));
    m_goldPriceSprite->setScale(0.5f);
    goldLayer->addChild(m_goldPriceSprite);
    updateUnlockedGoldLabel();

    cocos2d::CCLayer* shopLayer = cocos2d::CCLayer::create();
    pageLayers->addObject(shopLayer);
    cocos2d::CCMenu* shopMenu = cocos2d::CCMenu::create();
    shopLayer->addChild(shopMenu);

    for (int i = 1; i < 5; i++) {
        std::string ropeFrame = "shopRope2_001.png";
        std::string ropeFrame2 = "shopRope2_2_001.png";
        switch (i) {
        case 2: ropeFrame = "shopRope3_001.png"; ropeFrame2 = "shopRope3_2_001.png"; break;
        case 3: ropeFrame = "shopRope4_001.png"; ropeFrame2 = "shopRope4_2_001.png"; break;
        case 4: ropeFrame = "shopRope5_001.png"; ropeFrame2 = "shopRope5_2_001.png"; break;
        }

        cocos2d::CCSprite* rope = cocos2d::CCSprite::createWithSpriteFrameName(ropeFrame.c_str());
        CCMenuItemSpriteExtra* shopBtn = CCMenuItemSpriteExtra::create(
            rope, nullptr, menu_selector(SecretRewardsLayer::onShop));
        shopBtn->setTag(i);
        shopMenu->addChild(shopBtn);
        shopBtn->setSizeMult(1.5f);
        cocos2d::CCPoint shopBtnPos = cocos2d::CCPoint(
            (float)(i - 1) * 70.0f - 105.0f + winSize.width * 0.5f,
            winSize.height - 100.0f);
        cocos2d::CCPoint menuNodePos = cocos2d::CCNode::convertToNodeSpace(shopMenu, shopBtnPos);
        shopBtn->setPosition(menuNodePos);
        shopBtn->useAnimationType(CCMenuItemSpriteExtra::AnimationType::Move);
        shopBtn->m_offset = cocos2d::CCPoint(0, -15.0f);
        shopBtn->m_duration = 0.3f;
        shopBtn->m_unselectedDuration = 0.3f;

        cocos2d::CCPoint ropePos = cocos2d::CCPoint(25.0f, 76.0f);
        if (i == 2) ropePos.x -= 2.25f;
        else if (i == 3) ropePos.x -= 3.5f;

        for (int j = 0; j < 6; j++) {
            cocos2d::CCSprite* ropeDetail = cocos2d::CCSprite::createWithSpriteFrameName(ropeFrame2.c_str());
            rope->addChild(ropeDetail);
            ropeDetail->setPosition(ropePos + cocos2d::CCPoint(0.0f, (float)j * 20.5f));
        }
    }

    m_mainScrollLayer = BoomScrollLayer::create(pageLayers, 0, true, 0, 0, nullptr);
    m_mainLayer->addChild(m_mainScrollLayer);
    m_mainScrollLayer->setPagesIndicatorPosition(cocos2d::CCPoint(
        winSize.width * 0.5f, cocos2d::CCDirector::sharedDirector()->getScreenBottom() + 10.0f));
    m_mainScrollLayer->m_doVisit = true;
    m_mainScrollLayer->m_unkFloat2 = 0.9f;
    m_mainScrollLayer->m_clippingRect = cocos2d::CCRect(0, 0,
        cocos2d::CCDirector::sharedDirector()->getScreenRight(),
        cocos2d::CCDirector::sharedDirector()->getScreenTop());

    cocos2d::CCMenu* arrowMenu = cocos2d::CCMenu::create();
    arrowMenu->setTouchPriority(-129);
    float arrowY = winSize.height * 0.5f - 30.0f;

    cocos2d::CCSprite* leftArrowSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_leftButton = CCMenuItemSpriteExtra::create(
        leftArrowSprite, nullptr, menu_selector(SecretRewardsLayer::onSwitchPage));
    arrowMenu->addChild(m_leftButton);
    cocos2d::CCPoint leftPos = cocos2d::CCNode::convertToNodeSpace(arrowMenu,
        cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f, arrowY));
    m_leftButton->setPosition(leftPos);
    m_leftButton->setTag(0);
    m_leftButton->setSizeMult(2.0f);

    cocos2d::CCSprite* rightArrowSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    rightArrowSprite->setFlipX(true);
    m_rightButton = CCMenuItemSpriteExtra::create(
        rightArrowSprite, nullptr, menu_selector(SecretRewardsLayer::onSwitchPage));
    arrowMenu->addChild(m_rightButton);
    cocos2d::CCPoint rightPos = cocos2d::CCNode::convertToNodeSpace(arrowMenu,
        cocos2d::CCPoint(cocos2d::CCDirector::sharedDirector()->getScreenRight() - 24.0f, arrowY));
    m_rightButton->setPosition(rightPos);
    m_rightButton->setTag(1);
    m_rightButton->setSizeMult(2.0f);

    addChild(arrowMenu, 10);

    if (fromShop) {
        int totalPages = m_mainScrollLayer->getTotalPages();
        m_mainScrollLayer->instantMoveToPage(totalPages - 2);
        m_mainScrollLayer->instantMoveToPage(totalPages - 1);
    }

    return true;
}

void SecretRewardsLayer::keyBackClicked() {
    onBack(nullptr);
}

void SecretRewardsLayer::moveToMainLayer(cocos2d::CCObject* sender) {
    if (m_inMainLayer) {
        return;
    }

    m_inMainLayer = true;
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainScrollLayer->setTouchEnabled(true);
    m_mainLayer->runAction(cocos2d::CCEaseInOut::create(
        cocos2d::CCMoveTo::create(0.4f, cocos2d::CCPoint(0, 0)), 2.0f));

    if (m_secondaryLayer) {
        m_secondaryScrollLayer->setTouchEnabled(false);
        m_secondaryScrollLayer = nullptr;
        m_chestCounter = nullptr;

        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
            cocos2d::CCEaseInOut::create(
                cocos2d::CCMoveTo::create(0.4f, cocos2d::CCPoint(0, winSize.height + 50.0f)), 2.0f),
            cocos2d::CCCallFunc::create(m_secondaryLayer, callfunc_selector(cocos2d::CCNode::removeFromParent)),
            nullptr);
        m_secondaryLayer->runAction(seq);
        m_secondaryLayer = nullptr;
    }

    updateBackButton();
}

void SecretRewardsLayer::moveToSecondaryLayer(int page) {
    if (!m_inMainLayer) {
        return;
    }

    m_rewardType = page;
    m_inMainLayer = false;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_mainScrollLayer->setTouchEnabled(false);
    m_mainLayer->runAction(cocos2d::CCEaseBounceOut::create(
        cocos2d::CCMoveTo::create(0.5f, cocos2d::CCPoint(0, -winSize.height - 100.0f))));

    createSecondaryLayer(page);
    m_secondaryLayer->setPosition(cocos2d::CCPoint(0, winSize.height));
    m_secondaryLayer->runAction(cocos2d::CCEaseBounceOut::create(
        cocos2d::CCMoveTo::create(0.5f, cocos2d::CCPoint(0, 0))));

    updateBackButton();
}

void SecretRewardsLayer::onBack(cocos2d::CCObject* sender) {
    if (m_inMainLayer) {
        GameManager::sharedState()->fadeInMenuMusic();
        setKeypadEnabled(false);
        cocos2d::CCScene* scene = CreatorLayer::scene();
        cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
    } else {
        moveToMainLayer(sender);
    }
}

void SecretRewardsLayer::onChestType(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    if (tag <= 5) {
        moveToSecondaryLayer(tag + 3);
    } else {
        onSpecialItem(sender);
    }
}

void SecretRewardsLayer::onExit() {
    cocos2d::CCLayer::onExit();
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("TreasureRoomSheet.plist");
    cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey("TreasureRoomSheet.png");
}

void SecretRewardsLayer::onGoldChest(cocos2d::CCObject* sender) {
    int goldKeyStat = GameStatsManager::sharedState()->getStat("43");
    int nextChestID = GameStatsManager::getNextGoldChestID(GameStatsManager::sharedState());
    bool outOfChests = (nextChestID == 0);

    if (nextChestID && goldKeyStat > 0) {
        GJRewardItem* rewardItem = GameStatsManager::unlockGoldChest(
            GameStatsManager::sharedState(), nextChestID);
        if (rewardItem) {
            RewardUnlockLayer* layer = RewardUnlockLayer::create(rewardItem->m_rewardType, 0);
            addChild(layer, 100);
            RewardUnlockLayer::showCollectReward(layer, rewardItem);

            int newStat = GameStatsManager::sharedState()->getStat("43");
            m_goldKeysLabel->setString(cocos2d::CCString::createWithFormat("%i", newStat)->getCString());
            updateUnlockedGoldLabel();
            fadeInOutMusic();
        }
        return;
    }

    std::string message;
    int dialogIcon;
    int diff = 1 - goldKeyStat;
    if (outOfChests) {
        message = "All out of <cy>Gold Chests</c> right now. Come back later.";
    } else {
        message = "You need a <cy>Gold Key</c> to unlock this <cg>chest</c>.";
    }

    if (diff <= 50) {
        dialogIcon = 17;
        if (diff <= 25) {
            dialogIcon = (diff >= 11) ? 4 : 2;
        }
    } else {
        dialogIcon = 18;
    }

    DialogObject* dialogObj = DialogObject::create("The Keymaster", message.c_str(),
        dialogIcon, 1.0f, false, ccc3(255, 255, 255));
    DialogLayer* dialogLayer = DialogLayer::create(dialogObj, 2);
    DialogLayer::addToMainScene(dialogLayer);
    DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
    DialogLayer::animateInRandomSide(dialogLayer);
}

void SecretRewardsLayer::onSelectItem(cocos2d::CCObject* sender) {
    int keyStat = GameStatsManager::sharedState()->getStat("21");
    int tag = sender->getTag();
    int chestID = 0;
    int cost = GameStatsManager::keyCostForSecretChest(GameStatsManager::sharedState(), tag);

    if (keyStat >= cost) {
        GJRewardItem* rewardItem = GameStatsManager::unlockSecretChest(
            GameStatsManager::sharedState(), tag);
        if (rewardItem) {
            rewardItem->m_chestID = chestID;
            switchToOpenedState((CCMenuItemSpriteExtra*)sender);
            RewardUnlockLayer* layer = RewardUnlockLayer::create(rewardItem->m_rewardType, 0);
            addChild(layer, 100);
            RewardUnlockLayer::showCollectReward(layer, rewardItem);

            int newStat = GameStatsManager::sharedState()->getStat("21");
            m_keysLabel->setString(cocos2d::CCString::createWithFormat("%i", newStat)->getCString());
            updateUnlockedLabel();
        }
        return;
    }

    int missing = cost - keyStat;
    std::string message;
    if (cost == 1) {
        message = "You need a <cy>key</c> to unlock this <cg>chest</c>.";
    } else {
        message = cocos2d::CCString::createWithFormat(
            "You need <cl>%i</c> more <cy>keys</c> to unlock this <cg>chest</c>", missing)->getCString();
    }

    int dialogIcon;
    if (missing <= 50) {
        dialogIcon = 17;
        if (missing <= 25) {
            dialogIcon = (missing >= 11) ? 4 : 2;
        }
    } else {
        dialogIcon = 18;
    }

    DialogObject* dialogObj = DialogObject::create("The Keymaster", message.c_str(),
        dialogIcon, 1.0f, false, ccc3(255, 255, 255));
    DialogLayer* dialogLayer = DialogLayer::create(dialogObj, 2);
    DialogLayer::addToMainScene(dialogLayer);
    DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
    DialogLayer::animateInRandomSide(dialogLayer);
}

void SecretRewardsLayer::onShop(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    switch (tag) {
    case 1:
        if (GameManager::sharedState()->getUGV("11")) {
            showShop(1);
        } else {
            showDialog01();
        }
        break;
    case 2:
        if (GameManager::sharedState()->getUGV("20")) {
            showShop(2);
        } else {
            showDialog03();
        }
        break;
    case 3:
        if (GameManager::sharedState()->getUGV("35")) {
            showShop(3);
        } else {
            showDialogMechanic();
        }
        break;
    case 4:
        if (GameManager::sharedState()->getUGV("34")) {
            showShop(4);
        } else {
            showDialogDiamond();
        }
        break;
    default:
        break;
    }
}

void SecretRewardsLayer::onSpecialItem(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    int specialIdx = tag - 5;

    std::string chestKey;
    int requiredChests;
    switch (specialIdx) {
    case 1: chestKey = "0004"; requiredChests = 50; break;
    case 2: chestKey = "0005"; requiredChests = 100; break;
    case 3: chestKey = "0006"; requiredChests = 200; break;
    default: return;
    }

    int totalChests = GameStatsManager::sharedState()->m_treasureRoomChests->count();
    if (requiredChests <= totalChests) {
        GJRewardItem* rewardItem = GameStatsManager::unlockSpecialChest(
            GameStatsManager::sharedState(), chestKey);
        if (rewardItem) {
            switchToOpenedState((CCMenuItemSpriteExtra*)sender);
            RewardUnlockLayer* layer = RewardUnlockLayer::create(rewardItem->m_rewardType, 0);
            addChild(layer, 100);
            RewardUnlockLayer::showCollectReward(layer, rewardItem);
            return;
        }
    }

    int missing = requiredChests - totalChests;
    const char* chestWord = (missing == 1) ? "Chest" : "Chests";
    cocos2d::CCArray* objects = cocos2d::CCArray::create();
    std::string message = cocos2d::CCString::createWithFormat(
        "Unlock <cy>%i</c> more <cg>%s</c>, and you can collect this bonus.",
        missing, chestWord)->getCString();
    DialogObject* dialogObj = DialogObject::create("The Keymaster", message.c_str(),
        2, 1.0f, false, ccc3(255, 255, 255));
    objects->addObject(dialogObj);
    DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
    addChild(dialogLayer, 100);
    DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
    DialogLayer::animateInRandomSide(dialogLayer);
}

void SecretRewardsLayer::onSwitchPage(cocos2d::CCObject* sender) {
    if (!m_inMainLayer) {
        m_mainScrollLayer = m_secondaryScrollLayer;
    }
    int currentPage = m_mainScrollLayer->m_page;
    int direction = (sender->getTag() == 1) ? 1 : -1;
    goToPage(currentPage + direction);
}

void SecretRewardsLayer::scrollLayerMoved(cocos2d::CCPoint position) {
    if (!m_inMainLayer) {
        return;
    }

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    int totalPages = m_mainScrollLayer->getTotalPages();

    float pageFloat = -position.x / winSize.width;
    while (pageFloat < 0.0f) {
        pageFloat += totalPages;
    }

    int pageIdx = (int)pageFloat;
    float fraction = pageFloat - pageIdx;
    int nextPage = (pageIdx + 1) % totalPages;

    cocos2d::ccColor3B currentColor = getPageColor(pageIdx);
    cocos2d::ccColor3B nextColor = getPageColor(nextPage);

    float blend = fraction / 0.66667f - 0.2f;
    if (blend < 0.0f) blend = 0.0f;
    if (blend > 1.0f) blend = 1.0f;

    cocos2d::ccColor3B finalColor = GameToolbox::multipliedColorValue(currentColor, nextColor, blend);
    m_backgroundSprite->setColor(finalColor);

    if (m_inMainLayer) {
        int realPage = (int)llroundf(pageFloat) % totalPages;
        m_inMainLayer = (realPage != 3);
    }

    bool showKeys = m_inMainLayer;
    bool showGoldKeys = !m_inMainLayer;

    m_keysLabel->setVisible(showKeys);
    m_keySprite->setVisible(showKeys);
    m_goldKeysLabel->setVisible(showGoldKeys);
    m_goldKeySprite->setVisible(showGoldKeys);
}

void SecretRewardsLayer::showDialog01() {
    cocos2d::CCArray* objects = cocos2d::CCArray::create();
    int diamondStat = GameStatsManager::sharedState()->getStat("13");

    if (diamondStat > 499 && GameManager::sharedState()->getUGV("10")) {
        objects->addObject(DialogObject::create("Scratch",
            "Oh,<d020> the <cl>diamonds</c>.<d030>\nYou found them.",
            10, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "Alright come in, <d010>quickly before someone sees you.",
            13, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "You never know who might be watching.",
            8, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "<d010>.<d010>.<d010>.",
            9, 1.0f, false, ccc3(255, 255, 255)));

        DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
        addChild(dialogLayer, 100);
        dialogLayer->setTag(1);
        dialogLayer->m_delegate = this;
        DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
        DialogLayer::animateInRandomSide(dialogLayer);
        return;
    }

    switch (m_scratchDialogIndex) {
    case 0:
        objects->addObject(DialogObject::create("Scratch",
            "Uhm,<d030> there is no rope.", 8, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "Just<d010>.<d010>.<d010>. Pretend this never happened.", 9, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "<d010>.<d010>.<d010>.", 8, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 1:
        objects->addObject(DialogObject::create("Scratch",
            "I'm gonna need you to stop doing that.", 13, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "If RubRub sees this I am in <cr>BIG</c> trouble.", 8, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "I hate that guy<d010>.<d010>.<d010>.", 12, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 2:
        objects->addObject(DialogObject::create("Scratch",
            "Finally,<d020> I thought you wou<d010>.<d010>.<d010>.<d030>", 10, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "Wait,<d020> You're not the delivery guy.", 11, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "How am I supposed to run a shop if I don't have any merchandise?", 12, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "I mean, <d040>what shop?", 13, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "<d010>.<d010>.<d010>.", 8, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 3:
        objects->addObject(DialogObject::create("Scratch",
            "Look,<d030> I can't let you in.\n<d040>Not without the<d010>.<d010>.<d010>. <cl>stuff</c>.",
            8, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "I'm not even supposed to be here.\nI'm just a glitch in the code.",
            13, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "Did you hear something?", 8, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "<d010>.<d010>.<d010>.", 9, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 4:
        objects->addObject(DialogObject::create("Scratch",
            cocos2d::CCString::createWithFormat(
                "Alright, alright.<d010> If you collect\n<cy>%i</c> <cl>Diamonds</c> I will let you in.", 500)->getCString(),
            10, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            cocos2d::CCString::createWithFormat(
                "Usually the limit is %i, but you look a bit scary.", 500)->getCString(),
            13, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "Just, <d030>get out of here before someone sees you.", 11, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "Do you smell burning chicken?", 8, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "<d010>.<d010>.<d010>.", 9, 1.0f, false, ccc3(255, 255, 255)));
        GameManager::sharedState()->setUGV("10", true);
        break;
    case 5:
        objects->addObject(DialogObject::create("Scratch",
            cocos2d::CCString::createWithFormat(
                "Collect <cy>%i</c> <cl>Diamonds</c> and I will let you in.", 500)->getCString(),
            10, 0.8f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "Now go away, RubRub might be watching.", 8, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Scratch",
            "<d010>.<d010>.<d010>.", 9, 1.0f, false, ccc3(255, 255, 255)));
        break;
    default:
        return;
    }

    int nextIdx = m_scratchDialogIndex + 1;
    if (nextIdx >= 6) nextIdx = 0;
    m_scratchDialogIndex = nextIdx;

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
    addChild(dialogLayer, 100);
    dialogLayer->setTag(1);
    DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
    DialogLayer::animateInRandomSide(dialogLayer);
}

void SecretRewardsLayer::showDialog03() {
    cocos2d::CCArray* objects = cocos2d::CCArray::create();
    int diamondStat = GameStatsManager::sharedState()->getStat("13");

    if (diamondStat > 199 && GameManager::sharedState()->getUGV("21")) {
        objects->addObject(DialogObject::create("Potbor",
            "You found all the <cl>diamonds</c>!?", 21, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor",
            "Just need to check: \n<d050> <cl>1</c>, <d050><cl>2</c>, <d050><cl>3</c>, <d050><cy>200</c>. <d050>Yup, all there!",
            22, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor",
            "My first customer, how exciting!", 20, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor",
            "<d010>.<d010>.<d010>.", 19, 1.0f, false, ccc3(255, 255, 255)));

        DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
        addChild(dialogLayer, 100);
        dialogLayer->setTag(2);
        dialogLayer->m_delegate = this;
        DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
        DialogLayer::animateInRandomSide(dialogLayer);
        return;
    }

    switch (m_potborDialogIndex) {
    case 0:
        objects->addObject(DialogObject::create("Potbor", "Oh,<d030> a customer!",
            21, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor", "I never had a customer before.",
            22, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor", "<d010>.<d010>.<d010>.<d050> Not sure what to do.",
            19, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 1:
        objects->addObject(DialogObject::create("Potbor", "Let me check the manual.",
            24, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor",
            "<cg>The Shopkeepers handbook</c>: \n<d030><cy>You give stuff, they pay money.</c>",
            22, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor", "<d010>.<d010>.<d010>.<d050> Sounds simple enough.",
            23, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 2:
        objects->addObject(DialogObject::create("Potbor", "<cg>Page 2</c>: Refund policy\n<d030><cy>No refunds!</c>",
            25, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor",
            "<cg>Page 3</c>: Shopkeeper etiquette\n<cy><d010>.<d010>.<d010>. Probably not important.</c>",
            20, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor",
            "<cg>Page 4</c>: Self destruct mechanism\n<d030><cy>The page is missing...</c>",
            22, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor", "Not sure if I should be worried about this.",
            23, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 3:
        objects->addObject(DialogObject::create("Potbor",
            cocos2d::CCString::createWithFormat(
                "<cg>Page 394</c>: Customer must have <cy>%i</c> diamonds.", 200)->getCString(),
            24, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor", "What's a diamond?",
            22, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor", "<d010>.<d010>.<d010>.",
            20, 1.0f, false, ccc3(255, 255, 255)));
        GameManager::sharedState()->setUGV("21", true);
        break;
    case 4:
        objects->addObject(DialogObject::create("Potbor",
            cocos2d::CCString::createWithFormat(
                "You must collect <cy>%i</c> <cl>Diamonds</c> before I can let you in the <cg>Shop</c>!",
                200)->getCString(),
            25, 0.8f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor",
            "It says so in the handbook<d010>.<d010>.<d010>.",
            23, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Potbor", "<d010>.<d010>.<d010>.",
            20, 1.0f, false, ccc3(255, 255, 255)));
        break;
    default:
        return;
    }

    int nextIdx = m_potborDialogIndex + 1;
    if (nextIdx >= 5) nextIdx = 0;
    m_potborDialogIndex = nextIdx;

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
    addChild(dialogLayer, 100);
    dialogLayer->setTag(2);
    DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
    DialogLayer::animateInRandomSide(dialogLayer);
}

void SecretRewardsLayer::showDialogDiamond() {
    cocos2d::CCArray* objects = cocos2d::CCArray::create();
    int diamondStat = GameStatsManager::sharedState()->getStat("13");

    if (diamondStat > 999 && GameManager::sharedState()->getUGV("37")) {
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            "<cl>ShINe! <d040>sHiMmer!</c>", 43, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            "<cy>YOU!<d040> ShiNy!<d040> sParKLe!</c>", 41, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            "<cl><d010>.<d010>.<d010>.</c>", 48, 1.0f, false, ccc3(255, 255, 255)));

        DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
        addChild(dialogLayer, 100);
        dialogLayer->setTag(3);
        dialogLayer->m_delegate = this;
        DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
        DialogLayer::animateInRandomSide(dialogLayer);
        return;
    }

    switch (m_diamondDialogIndex) {
    case 0:
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            "<cl>gLiTz sCraPe sParKLe TwiNkLe<d010>.<d010>.<d010>.</c>",
            42, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            "<cl>.<d010>.<d010>.<d010>.</c>", 40, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 1:
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            "<cl>gRasP sHinEe tWinKleR<d010>.<d010>.<d010>. <d050>cLiNk!</c>",
            43, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            "<cy><i050><s260>bRinG</s> <d040><s260>ShiNy!</c></s></i>",
            41, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 2:
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            "<cl>sHimMeR glOrP<d010>.<d010>.<d010>. cRyStAl sHine!</c>",
            42, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            cocos2d::CCString::createWithFormat(
                "<cg>%i</c><d010>.<d010>.<d010>. <cy><i050><s260>bRinG</s> <d040><s260>ShiNy!</c></s></i>",
                1000)->getCString(),
            41, 1.0f, false, ccc3(255, 255, 255)));
        GameManager::sharedState()->setUGV("37", true);
        break;
    case 3:
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            cocos2d::CCString::createWithFormat(
                "<cy>%i</c><cl><d010>.<d010>.<d010>. gLiSten gLoRp sPaRkle?</c>", 1000)->getCString(),
            47, 0.8f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("Diamond Shopkeeper",
            "<cl>.<d010>.<d010>.<d020>.<d030>.<d050></c> <cy>sHiNY</c><cl>?</c>",
            48, 1.0f, false, ccc3(255, 255, 255)));
        break;
    default:
        return;
    }

    int nextIdx = m_diamondDialogIndex + 1;
    if (nextIdx >= 4) nextIdx = 0;
    m_diamondDialogIndex = nextIdx;

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
    addChild(dialogLayer, 100);
    dialogLayer->setTag(3);
    DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
    DialogLayer::animateInRandomSide(dialogLayer);
}

void SecretRewardsLayer::showDialogMechanic() {
    cocos2d::CCArray* objects = cocos2d::CCArray::create();
    int diamondStat = GameStatsManager::sharedState()->getStat("13");

    if (diamondStat > 1999 && GameManager::sharedState()->getUGV("36")) {
        objects->addObject(DialogObject::create("The Mechanic",
            "Now that's an impressive collection of <cl>Diamonds</c>!",
            39, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "If anyone asks I didn't let you in.", 37, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "Scratch keeps trying to sneak past my security<d010>.<d010>.<d010>.",
            50, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "<i050><s260><co>I CAN'T SEE!?</c></s></i>", 44, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "Ah sorry, <d020>my <cg>goggles</c> were off.\n<co>Come in</c>.",
            51, 1.0f, false, ccc3(255, 255, 255)));

        DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
        addChild(dialogLayer, 100);
        dialogLayer->setTag(4);
        dialogLayer->m_delegate = this;
        DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
        DialogLayer::animateInRandomSide(dialogLayer);
        return;
    }

    switch (m_mechanicDialogIndex) {
    case 0:
        objects->addObject(DialogObject::create("The Mechanic",
            "I'm in the middle of a job.\nCome back later.", 38, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "<cl>Can never get any peace and quiet in this game...</c>",
            37, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 1:
        objects->addObject(DialogObject::create("The Mechanic",
            "<co>You again!? What now!?</c>", 46, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "I'm busy go bother someone else.", 51, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "<cl>You take one job then all of a sudden you're bugfixing the whole game.</c>",
            37, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 2:
        objects->addObject(DialogObject::create("The Mechanic",
            "<cl>Hmm so if I push here<d010>.<d010>.<d010>.<d040> maybe...</c>",
            45, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "<cl>No, still broken...</c>", 49, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 3:
        objects->addObject(DialogObject::create("The Mechanic",
            "I have enough work as it is, can't go around helping everyone.",
            51, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            cocos2d::CCString::createWithFormat(
                "Collect <cy>%i</c> <cl>Diamonds</c> so I know you're legit, then we can talk.",
                2000)->getCString(),
            37, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "<cl>Who ordered this thing it doesn't even fit...</c>",
            49, 1.0f, false, ccc3(255, 255, 255)));
        GameManager::sharedState()->setUGV("36", true);
        break;
    case 4:
        objects->addObject(DialogObject::create("The Mechanic",
            "<cg>Scanning user<d010>.<d010>.<d010>.</c>", 49, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            cocos2d::CCString::createWithFormat(
                "<cg>Scan complete.</c>\nDiamonds: <cc>%i</c>, Required: <cy>%i</c>",
                diamondStat, 2000)->getCString(),
            45, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "<co>Hey who are you trying to fool?!</c>", 46, 1.0f, false, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Mechanic",
            "<cg>Come back</c> when you have\n<cy>collected</c> more <cl>Diamonds</c><d010>.<d010>.<d010>.",
            50, 1.0f, false, ccc3(255, 255, 255)));
        break;
    default:
        return;
    }

    int nextIdx = m_mechanicDialogIndex + 1;
    if (nextIdx >= 5) nextIdx = 0;
    m_mechanicDialogIndex = nextIdx;

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
    addChild(dialogLayer, 100);
    dialogLayer->setTag(4);
    DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
    DialogLayer::animateInRandomSide(dialogLayer);
}

void SecretRewardsLayer::showLockedChest() {
    std::string message;
    int dialogIcon = 2;

    switch (m_lockedDialogIndex) {
    case 0: message = "The <cg>chest</c> is not ready."; break;
    case 1: message = "It's still not ready.<d030>.<d030>."; break;
    case 2: message = "Seriously, <d040><cr>stop</c>."; break;
    case 3: message = "Wow it magically opened!"; dialogIcon = 17; break;
    case 4: message = "I was wrong."; break;
    case 5: message = "<co>S <d020>T <d020>O <d020>P</c>"; break;
    case 6: message = "El cofre esta cerrado."; break;
    case 7: message = "This is getting ridiculous."; break;
    case 8: message = "I really can't help you."; break;
    case 9: message = "Don't make me do it..."; break;
    case 10:
        message = "<cr>Roses are red.</c> <d060><cl>Violets are blue.</c> <d060><cg>This is not.</c> <d060><cy>2.2</c>";
        break;
    default:
        return;
    }

    DialogObject* dialogObj = DialogObject::create("The Keymaster", message.c_str(),
        dialogIcon, 1.0f, false, ccc3(255, 255, 255));
    DialogLayer* dialogLayer = DialogLayer::create(dialogObj, 2);
    DialogLayer::addToMainScene(dialogLayer);
    DialogLayer::updateChatPlacement(dialogLayer, DialogLayer::Center);
    DialogLayer::animateInRandomSide(dialogLayer);

    int nextIdx = m_lockedDialogIndex + 1;
    if (nextIdx > 10) nextIdx = 0;
    m_lockedDialogIndex = nextIdx;
}

void SecretRewardsLayer::showShop(int shop) {
    cocos2d::CCScene* scene = GJShopLayer::scene((ShopType)shop);
    cocos2d::CCTransitionMoveInT* transition = cocos2d::CCTransitionMoveInT::create(0.5f, scene);
    if (cocos2d::CCDirector::sharedDirector()->replaceScene(transition)) {
        setKeypadEnabled(false);
    }
}

void SecretRewardsLayer::switchToOpenedState(CCMenuItemSpriteExtra* button) {
    GJChestSprite* chest = (GJChestSprite*)button->getNormalImage();
    chest->switchToState(GJChestSprite::ChestState::Opened, true);

    cocos2d::CCArray* children = chest->getChildren();
    if (children) {
        for (unsigned int i = 0; i < children->count(); i++) {
            cocos2d::CCNode* child = (cocos2d::CCNode*)children->objectAtIndex(i);
            child->setOpacity(100);
            child->setColor(ccc3(150, 150, 150));
        }
    }

    button->setEnabled(false);
}

void SecretRewardsLayer::updateBackButton() {
    m_backSprite->stopAllActions();

    const char* frameName = m_inMainLayer ? "GJ_arrow_03_001.png" : "GJ_closeBtn_001.png";
    cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName);
    m_backSprite->setDisplayFrame(frame);

    if (!m_inMainLayer) {
        m_backSprite->setScale(0.8f);
    } else {
        m_backSprite->setScale(1.0f);
    }

    if (!m_inMainLayer) {
        float scale = m_backSprite->getScale();
        cocos2d::CCActionInterval* scaleUp = cocos2d::CCEaseBackOut::create(
            cocos2d::CCScaleTo::create(0.2f, scale + 0.1f));
        cocos2d::CCActionInterval* scaleDown = cocos2d::CCEaseInOut::create(
            cocos2d::CCScaleTo::create(0.2f, scale), 2.0f);
        m_backSprite->runAction(cocos2d::CCSequence::create(scaleUp, scaleDown, nullptr));
    }
}

void SecretRewardsLayer::updateUnlockedGoldLabel() {
    int unlocked = GameStatsManager::countUnlockedSecretChests(
        GameStatsManager::sharedState(), GJRewardType::Gold);
    int total = GameStatsManager::countSecretChests(
        GameStatsManager::sharedState(), GJRewardType::Gold);

    m_goldPriceSprite->setString(cocos2d::CCString::createWithFormat(
        "%i / %i", unlocked, total)->getCString());

    if (unlocked >= total) {
        m_goldPriceSprite->setColor(ccc3(50, 255, 50));
    } else {
        m_goldPriceSprite->setColor(ccc3(100, 255, 100));
    }
}

void SecretRewardsLayer::updateUnlockedLabel() {
    int unlocked = GameStatsManager::countUnlockedSecretChests(
        GameStatsManager::sharedState(), (GJRewardType)0);
    int total = GameStatsManager::countSecretChests(
        GameStatsManager::sharedState(), (GJRewardType)0);

    if (m_chestLabel) {
        m_chestLabel->setString(cocos2d::CCString::createWithFormat(
            "%i/%i", unlocked, total)->getCString());
    }

    if (m_inMainLayer) {
        return;
    }

    int unlockedType = GameStatsManager::countUnlockedSecretChests(
        GameStatsManager::sharedState(), (GJRewardType)m_rewardType);
    int totalType = GameStatsManager::countSecretChests(
        GameStatsManager::sharedState(), (GJRewardType)m_rewardType);

    std::string counterStr = cocos2d::CCString::createWithFormat(
        "%i/%i", unlockedType, totalType)->getCString();

    if (m_chestCounter) {
        m_chestCounter->setString(counterStr.c_str());
    }

    cocos2d::CCObject* counterObj = m_chestCounters->objectForKey(
        cocos2d::CCString::createWithFormat("%i", m_rewardType)->getCString());
    if (counterObj) {
        ((cocos2d::CCLabelBMFont*)counterObj)->setString(counterStr.c_str());
    }

    if (unlockedType >= totalType) {
        if (m_chestCounter) {
            m_chestCounter->setColor(ccc3(0, 255, 255));
        }
        if (counterObj) {
            ((cocos2d::CCLabelBMFont*)counterObj)->setColor(ccc3(50, 255, 50));
        }
    }
}

SecretRewardsLayer::~SecretRewardsLayer() {
}