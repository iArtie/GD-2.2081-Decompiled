#include "Pages/ShardsPage.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "UI/GJItemIcon.h"
#include "Popups/ItemInfoPopup.h"

void ShardsPage::FLAlert_Clicked(FLAlertLayer* alert, bool btn2) {
}

ShardsPage* ShardsPage::create() {
    ShardsPage* returnVal = new ShardsPage();
    if (returnVal->init()) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

void ShardsPage::goToPage(int page) {
    if ((unsigned int)page > 3 || m_page == page)
        return;

    m_page = page;
    int count = m_pages->count();
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            cocos2d::CCArray* array = (cocos2d::CCArray*)m_pages->objectForKey(i);
            if (array) {
                unsigned int num = array->count();
                for (unsigned int j = 0; j < num; j++) {
                    cocos2d::CCObject* obj = array->objectAtIndex(j);
                    if (obj) {
                        obj->setVisible(page == i);
                    }
                }
            }
        }
    }

    m_prevButton->setVisible(page > 0);
    m_nextButton->setVisible(page <= 2);

    const char* frameName;
    if (page <= 1)
        frameName = "tier1Icon_001.png";
    else
        frameName = "tier2Icon_001.png";

    m_tierSprite->setDisplayFrame(
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName));
}

bool ShardsPage::init() {
    if (!FLAlertLayer::init(150))
        return false;

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;

    cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
    bg->setContentSize(cocos2d::CCSize(420.0f, 280.0f));
    bg->setPosition(centerX, centerY);
    m_mainLayer->addChild(bg, -1);

    for (int i = 1; i <= 4; i++) {
        cocos2d::CCSprite* corner = cocos2d::CCSprite::createWithSpriteFrameName("dailyLevelCorner_001.png");
        m_mainLayer->addChild(corner, 8);
        if (i == 3) {
            corner->setFlipX(true);
            corner->setFlipY(true);
            corner->setPosition(centerX + 185.0f, centerY + 115.0f);
        } else if (i == 2) {
            corner->setFlipY(true);
            corner->setPosition(centerX - 185.0f, centerY + 115.0f);
        } else if (i == 4) {
            corner->setFlipX(true);
            corner->setPosition(centerX + 185.0f, centerY - 115.0f);
        } else {
            corner->setPosition(centerX - 185.0f, centerY - 115.0f);
        }
    }

    int intVal14 = -1;
    int intVal15 = -1;

    cocos2d::CCPoint point4(centerX, centerY + 140.0f - 28.0f);
    cocos2d::CCSprite* labelShards = cocos2d::CCSprite::createWithSpriteFrameName("label_shards_001.png");
    labelShards->setPosition(point4 + cocos2d::CCPoint(-80.0f, 0.0f));
    m_mainLayer->addChild(labelShards);

    cocos2d::CCSprite* labelOf = cocos2d::CCSprite::createWithSpriteFrameName("label_of_001.png");
    labelOf->setPosition(point4 + cocos2d::CCPoint(4.0f, 0.0f));
    m_mainLayer->addChild(labelOf);

    cocos2d::CCSprite* labelPower = cocos2d::CCSprite::createWithSpriteFrameName("label_power_001.png");
    labelPower->setPosition(point4 + cocos2d::CCPoint(85.0f, 0.0f));
    m_mainLayer->addChild(labelPower);

    cocos2d::CCPoint point5(centerX, centerY + 80.0f);

    m_pages = cocos2d::CCDictionary::create();
    m_pages->retain();

    cocos2d::CCSprite* closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
    CCMenuItemSpriteExtra* closeBtn = CCMenuItemSpriteExtra::create(
        closeSprite, nullptr, menu_selector(ShardsPage::onClose));
    closeBtn->setSizeMult(1.6f);
    m_buttonMenu = cocos2d::CCMenu::create(closeBtn, nullptr);
    m_buttonMenu->setPosition(centerX - 210.0f + 10.0f, centerY + 140.0f - 10.0f);
    m_mainLayer->addChild(m_buttonMenu, 10);

    cocos2d::CCSprite* playerSquare = cocos2d::CCSprite::createWithSpriteFrameName("playerSquare_001.png");

    for (int intVal17 = 0; intVal17 < 12; intVal17++) {
        int intVal2 = intVal17 / 3;
        cocos2d::CCArray* array = (cocos2d::CCArray*)m_pages->objectForKey(intVal2);
        if (!array) {
            array = cocos2d::CCArray::create();
            m_pages->setObject(array, intVal2);
        }

        std::string labelFrame;
        std::string smallFrame;
        std::string shardKey;
        int intVal16 = 0;
        cocos2d::ccColor3B color1 = {255, 255, 255};
        cocos2d::ccColor3B color2 = {255, 255, 255};
        cocos2d::ccColor3B barColor = {255, 255, 255};
        cocos2d::ccColor3B charVal = {255, 255, 255};
        cocos2d::ccColor3B charVal2 = {255, 255, 255};
        cocos2d::ccColor3B charVal3 = {255, 255, 255};

        switch (intVal17) {
            case 1:
                labelFrame = "iceShardLabel_001.png";
                smallFrame = "iceShardSmall_001.png";
                intVal16 = GameStatsManager::sharedState()->getStat("19");
                color1 = {255, 160, 0};
                color2 = {0, 255, 255};
                shardKey = "shardIce";
                if (intVal16 < intVal15 || intVal15 == -1)
                    intVal15 = intVal16;
                break;
            case 2:
                labelFrame = "poisonShardLabel_001.png";
                smallFrame = "poisonShardSmall_001.png";
                intVal16 = GameStatsManager::sharedState()->getStat("17");
                color1 = {0, 160, 0};
                color2 = {0, 0, 0};
                shardKey = "shardPoison";
                if (intVal16 < intVal15 || intVal15 == -1)
                    intVal15 = intVal16;
                break;
            case 3:
                labelFrame = "shadowShardLabel_001.png";
                smallFrame = "shadowShardSmall_001.png";
                intVal16 = GameStatsManager::sharedState()->getStat("16");
                color1 = {150, 200, 150};
                color2 = {50, 255, 255};
                shardKey = "shardShadow";
                if (intVal16 < intVal15 || intVal15 == -1)
                    intVal15 = intVal16;
                break;
            case 4:
                labelFrame = "lavaShardLabel_001.png";
                smallFrame = "lavaShardSmall_001.png";
                intVal16 = GameStatsManager::sharedState()->getStat("20");
                color1 = {175, 0, 25};
                color2 = {50, 50, 25};
                shardKey = "shardLava";
                if (intVal16 < intVal15 || intVal15 == -1)
                    intVal15 = intVal16;
                break;
            case 5:
                labelFrame = "bonusShardLabel_001.png";
                smallFrame = "bonusShardSmall_001.png";
                color1 = {150, 150, 0};
                shardKey = "shardBonus";
                intVal16 = intVal15;
                if (intVal15 == -1)
                    intVal15 = intVal16;
                break;
            case 6:
                labelFrame = "shard0201ShardLabel_001.png";
                smallFrame = "shard0201ShardSmall_001.png";
                color1 = {63, 252, 70};
                intVal16 = GameStatsManager::sharedState()->getStat("23");
                color2 = {61, 224, 160};
                shardKey = "shardEarth";
                if (intVal16 < intVal14 || intVal14 == -1)
                    intVal14 = intVal16;
                break;
            case 7:
                labelFrame = "shard0202ShardLabel_001.png";
                smallFrame = "shard0202ShardSmall_001.png";
                intVal16 = GameStatsManager::sharedState()->getStat("24");
                color1 = {150, 0, 0};
                color2 = {0, 0, 0};
                shardKey = "shardBlood";
                if (intVal16 < intVal14 || intVal14 == -1)
                    intVal14 = intVal16;
                break;
            case 8:
                labelFrame = "shard0203ShardLabel_001.png";
                smallFrame = "shard0203ShardSmall_001.png";
                intVal16 = GameStatsManager::sharedState()->getStat("25");
                color1 = {128, 128, 128};
                shardKey = "shardMetal";
                if (intVal16 < intVal14 || intVal14 == -1)
                    intVal14 = intVal16;
                break;
            case 9:
                labelFrame = "shard0204ShardLabel_001.png";
                smallFrame = "shard0204ShardSmall_001.png";
                intVal16 = GameStatsManager::sharedState()->getStat("26");
                color1 = {255, 255, 127};
                color2 = {0, 0, 64};
                shardKey = "shardLight";
                if (intVal16 < intVal14 || intVal14 == -1)
                    intVal14 = intVal16;
                break;
            case 10:
                labelFrame = "shard0205ShardLabel_001.png";
                smallFrame = "shard0205ShardSmall_001.png";
                intVal16 = GameStatsManager::sharedState()->getStat("27");
                color1 = {100, 0, 160};
                color2 = {0, 125, 125};
                shardKey = "shardSoul";
                if (intVal16 < intVal14 || intVal14 == -1)
                    intVal14 = intVal16;
                break;
            case 11:
                labelFrame = "bonusShardLabel_001.png";
                smallFrame = "bonusShard2Small_001.png";
                color1 = {150, 150, 0};
                shardKey = "shardBonusB";
                intVal16 = intVal14;
                if (intVal14 == -1)
                    intVal14 = intVal16;
                break;
            default:
                labelFrame = "fireShardLabel_001.png";
                smallFrame = "fireShardSmall_001.png";
                intVal16 = GameStatsManager::sharedState()->getStat("18");
                color1 = {75, 56, 0};
                color2 = {0, 0, 0};
                shardKey = "shardFire";
                if (intVal16 < intVal15 || intVal15 == -1)
                    intVal15 = intVal16;
                break;
        }

        cocos2d::CCPoint point6 = point5 + cocos2d::CCPoint(0.0f, (float)(-3 * intVal2 + intVal17) * -77.0f);

        cocos2d::CCSprite* sprite6 = cocos2d::CCSprite::createWithSpriteFrameName(labelFrame.c_str());
        m_mainLayer->addChild(sprite6);
        sprite6->setPosition(point6 + cocos2d::CCPoint(-20.0f, 0.0f));
        array->addObject(sprite6);

        cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create(
            cocos2d::CCString::createWithFormat("%i", intVal16)->getCString(), "bigFont.fnt");
        label->setScale(0.45f);
        m_mainLayer->addChild(label);
        label->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        label->setPosition(sprite6->getPosition().x + sprite6->getContentSize().width * 0.5f + 5.0f,
                           sprite6->getPosition().y + 1.0f);
        array->addObject(label);

        cocos2d::CCSprite* sprite7 = cocos2d::CCSprite::createWithSpriteFrameName(smallFrame.c_str());
        m_mainLayer->addChild(sprite7);
        sprite7->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        sprite7->setPosition(label->getPosition().x + label->getScaledContentSize().width + 2.0f,
                             label->getPosition().y);
        array->addObject(sprite7);

        for (int intVal3 = 1; intVal3 <= 5; intVal3++) {
            int intVal18 = *((int*)&unk_EFA290 + (intVal3 - 1));
            std::string achStr = cocos2d::CCString::createWithFormat("geometry.ach.%s%02d", shardKey.c_str(), intVal3)->getCString();

            UnlockType type = (UnlockType)0;
            int id = 0;
            GameManager::sharedState()->getUnlockForAchievement(achStr, &id, &type);

            if ((int)type > 0) {
                GJItemIcon* icon = GJItemIcon::create(type, id, color1, color2, false, false, false, {255, 255, 255});
                icon->setScale(0.8f);

                if (intVal16 < intVal18) {
                    icon->changeToLockedState(1.25f);
                    icon->darkenStoreItem(cocos2d::ccc3(87, 52, 29));
                }

                CCMenuItemSpriteExtra* menuItem2 = CCMenuItemSpriteExtra::create(
                    icon, nullptr, menu_selector(ShardsPage::onIconInfo));
                m_buttonMenu->addChild(menuItem2);
                menuItem2->setContentSize(playerSquare->getContentSize());
                icon->setPosition(menuItem2->getContentSize().width * 0.5f,
                                  menuItem2->getContentSize().height * 0.5f);
                menuItem2->setTag(intVal17);

                cocos2d::CCPoint point7 = point6 + cocos2d::CCPoint((float)(intVal3 - 3) * 70.0f, -30.0f);
                cocos2d::CCPoint worldPos = cocos2d::CCNode::convertToWorldSpace(m_mainLayer, &point7);
                cocos2d::CCPoint menuPos = cocos2d::CCNode::convertToNodeSpace(m_buttonMenu, &worldPos);
                menuItem2->setPosition(menuPos);
                array->addObject(menuItem2);

                if (intVal16 < intVal18) {
                    cocos2d::CCLabelBMFont* label2 = cocos2d::CCLabelBMFont::create(
                        cocos2d::CCString::createWithFormat("%i", intVal18)->getCString(), "bigFont.fnt");
                    label2->setScale(0.3f);
                    m_mainLayer->addChild(label2);
                    label2->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
                    float value9 = (intVal18 > 9) ? -10.0f : -8.0f;
                    label2->setPosition(point7 + cocos2d::CCPoint(value9, -22.0f));
                    label2->setColor(charVal);
                    array->addObject(label2);

                    cocos2d::CCSprite* sprite13 = cocos2d::CCSprite::createWithSpriteFrameName(smallFrame.c_str());
                    m_mainLayer->addChild(sprite13);
                    sprite13->setScale(0.7f);
                    sprite13->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
                    sprite13->setPosition(label2->getPosition().x + label2->getScaledContentSize().width + 2.0f,
                                          label2->getPosition().y);
                    array->addObject(sprite13);
                }

                if (intVal3 > 1) {
                    cocos2d::CCPoint point8 = point7 + cocos2d::CCPoint(-51.0f, 0.0f);
                    cocos2d::CCSprite* bar = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");
                    bar->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
                    bar->setPosition(point8);
                    m_mainLayer->addChild(bar, 0);
                    bar->setScaleX(32.0f / bar->getContentSize().width);
                    bar->setScaleY(5.0f / bar->getContentSize().height);
                    bar->setColor(ccc3(255, 255, 255));
                    bar->setOpacity(110);
                    array->addObject(bar);

                    int intVal7 = 0;
                    if ((unsigned int)(intVal3 - 2) <= 3)
                        intVal7 = *((int*)&unk_EFA290 + (unsigned int)(intVal3 - 2) + 12);
                    float value6 = (float)(intVal16 - intVal7) / (float)(intVal18 - intVal7);
                    if (value6 >= 1.0f)
                        value6 = 1.0f;

                    cocos2d::CCSprite* barFill = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");
                    barFill->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
                    barFill->setPosition(point8 + cocos2d::CCPoint(2.0f, 0.0f));
                    m_mainLayer->addChild(barFill, 1);
                    barFill->setScaleX((value6 * 28.0f) / barFill->getContentSize().width);
                    barFill->setScaleY(2.0f / barFill->getContentSize().height);
                    barFill->setColor(barColor);
                    array->addObject(barFill);
                }
            }
        }
    }

    cocos2d::CCMenu* menu2 = cocos2d::CCMenu::create();
    cocos2d::CCSprite* prevSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    m_prevButton = CCMenuItemSpriteExtra::create(prevSprite, nullptr, menu_selector(ShardsPage::onSwitchPage));
    menu2->addChild(m_prevButton);
    m_prevButton->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f, centerY));
    m_prevButton->setTag(0);
    m_prevButton->setSizeMult(1.4f);

    cocos2d::CCSprite* nextSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSprite->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(nextSprite, nullptr, menu_selector(ShardsPage::onSwitchPage));
    menu2->addChild(m_nextButton);
    m_nextButton->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenRight() - 24.0f, centerY));
    m_nextButton->setTag(1);
    m_nextButton->setSizeMult(1.4f);

    m_mainLayer->addChild(menu2, 10);

    m_tierSprite = cocos2d::CCSprite::createWithSpriteFrameName("tier1Icon_001.png");
    m_mainLayer->addChild(m_tierSprite, 10);
    m_tierSprite->setPosition(centerX + 210.0f - 18.0f, centerY + 140.0f - 18.0f);

    goToPage(0);
    return true;
}

void ShardsPage::keyBackClicked() {
    onClose(nullptr);
}

void ShardsPage::onClose(cocos2d::CCObject* sender) {
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
}

void ShardsPage::onIconInfo(cocos2d::CCObject* sender) {
    CCMenuItemSpriteExtra* item = (CCMenuItemSpriteExtra*)sender;
    GJItemIcon* icon = (GJItemIcon*)item->getNormalImage();
    ItemInfoPopup* popup = ItemInfoPopup::create(icon->m_unlockID, icon->m_unlockType);
    popup->show();
}

void ShardsPage::onSwitchPage(cocos2d::CCObject* sender) {
    int tag = sender->getTag();
    int newPage = (tag == 1) ? m_page + 1 : m_page - 1;
    goToPage(newPage);
}

void ShardsPage::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
}

void ShardsPage::show() {
    if (!m_noElasticity) {
        unsigned char opacity = getOpacity();
        cocos2d::CCPoint pos = m_mainLayer->getPosition();
        m_mainLayer->setPosition(cocos2d::CCPoint(-160.0f, pos.y));
        m_mainLayer->runAction(cocos2d::CCEaseElasticOut::create(
            cocos2d::CCMoveTo::create(0.5f, pos), 0.6f));
        cocos2d::CCScene* runningScene = cocos2d::CCDirector::sharedDirector()->m_pRunningScene;
        if (!m_scene) {
            int highestZ = cocos2d::CCScene::getHighestChildZ(runningScene);
            m_ZOrder = (highestZ > 104) ? highestZ + 1 : 105;
            m_scene = runningScene;
        } else if (!m_ZOrder) {
            m_ZOrder = 105;
        }
        m_scene->addChild(this);
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
        setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

ShardsPage::~ShardsPage() {
}