#include "Pages/GJPathPage.h"
#include "Layers/GJPathsLayer.h"
#include "Sprites/GJPathSprite.h"
#include "UI/GJItemIcon.h"
#include "Managers/GameStatsManager.h"
#include "Managers/GameManager.h"
#include "Managers/AchievementManager.h"
#include "Models/GJStoreItem.h"
#include "Popups/PurchaseItemPopup.h"
#include "Popups/ItemInfoPopup.h"
#include "Audio/FMODAudioEngine.h"
#include "Utils/GameToolbox.h"
#include "Layers/DialogLayer.h"
#include "Models/DialogObject.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "UI/InfoAlertButton.h"
#include "Cocos/CCSpriteGrayscale.h"
#include "Cocos/CCSpriteWithHue.h"

GJPathPage* GJPathPage::create(int path, GJPathsLayer* layer) {
    auto page = new GJPathPage();
    if (page && page->init(path, layer)) {
        page->autorelease();
        return page;
    }
    delete page;
    return nullptr;
}

bool GJPathPage::init(int path, GJPathsLayer* layer) {
    m_pathsLayer = layer;
    m_pathNumber = path;
    int itemId = path + 5;
    
    auto stats = GameStatsManager::sharedState();
    bool isUnlocked = stats->isItemUnlocked(GJItem, itemId);
    
    GJPathsLayer::darkenButtons(layer, true);
    FLAlertLayer::incrementForcePrio();
    
    if (!FLAlertLayer::initWithColor(cocos2d::ccc4(0, 0, 0, 150))) {
        return false;
    }
    
    setTouchEnabled(true);
    setKeypadEnabled(true);
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    float centerX = winSize.width * 0.5f;
    float centerY = winSize.height * 0.5f;
    
    m_mainLayer = cocos2d::CCLayer::create();
    addChild(m_mainLayer);
    
    cocos2d::ccColor3B bgColor = cocos2d::ccc3(255, 255, 255);
    cocos2d::ccColor3B progressColor = cocos2d::ccc3(50, 50, 50);
    
    // Set path-specific colors
    switch (path) {
        case 1:
            bgColor = cocos2d::ccc3(255, 130, 74);
            progressColor = cocos2d::ccc3(28, 75, 56);
            break;
        case 2:
            bgColor = cocos2d::ccc3(59, 133, 207);
            progressColor = cocos2d::ccc3(45, 29, 12);
            break;
        case 3:
            bgColor = cocos2d::ccc3(73, 160, 56);
            progressColor = cocos2d::ccc3(35, 16, 12);
            break;
        case 4:
            bgColor = cocos2d::ccc3(143, 79, 194);
            progressColor = cocos2d::ccc3(17, 31, 42);
            break;
        case 5:
            bgColor = cocos2d::ccc3(196, 74, 81);
            progressColor = cocos2d::ccc3(13, 16, 43);
            break;
        case 6:
            bgColor = cocos2d::ccc3(165, 98, 70);
            progressColor = cocos2d::ccc3(11, 18, 36);
            break;
        case 7:
            bgColor = cocos2d::ccc3(230, 34, 34);
            progressColor = cocos2d::ccc3(7, 7, 50);
            break;
        case 8:
            bgColor = cocos2d::ccc3(204, 204, 204);
            progressColor = cocos2d::ccc3(44, 44, 44);
            break;
        case 9:
            bgColor = cocos2d::ccc3(255, 178, 79);
            progressColor = cocos2d::ccc3(17, 39, 56);
            break;
        case 10:
            bgColor = cocos2d::ccc3(194, 69, 153);
            progressColor = cocos2d::ccc3(33, 15, 42);
            break;
        default:
            bgColor = cocos2d::ccc3(100, 100, 100);
            progressColor = cocos2d::ccc3(50, 50, 50);
            break;
    }
    
    CCSpriteGrayscale* bgSprite;
    if (isUnlocked) {
        bgSprite = static_cast<CCSpriteGrayscale*>(cocos2d::CCSprite::createWithSpriteFrameName("pathInnerBG_001.png"));
    } else {
        bgSprite = CCSpriteGrayscale::createWithSpriteFrameName("pathInnerBG_001.png");
        bgSprite->setEvenLuminance(2.5f);
    }
    
    m_mainLayer->addChild(bgSprite, -1);
    bgSprite->setScale(1.3f);
    bgSprite->setPosition(cocos2d::CCPoint(centerX, centerY));
    bgSprite->setColor(bgColor);
    
    float bgWidth = bgSprite->getContentSize().width * bgSprite->getScale();
    float bgHeight = bgSprite->getContentSize().height * bgSprite->getScale();
    
    auto backSprite = cocos2d::CCSprite::createWithSpriteFrameName("backArrowPlain_01_001.png");
    auto backItem = CCMenuItemSpriteExtra::create(backSprite, this, menu_selector(GJPathPage::onBack));
    backItem->setSizeMult(1.6f);
    
    m_buttonMenu = cocos2d::CCMenu::create(backItem, nullptr);
    m_buttonMenu->setPosition(cocos2d::CCPoint(
        12.0f - (bgWidth * 0.5f - centerX),
        bgHeight * 0.5f + centerY - 12.0f
    ));
    m_mainLayer->addChild(m_buttonMenu, 10);
    
    auto infoTitle = cocos2d::CCString::create("Paths of Power");
    auto infoDesc = cocos2d::CCString::create(
        "When you activate a path, <cy>Stars</c> and <cl>Moons</c> you <cg>collect</c> will level up your <co>Path</c>. You can unlock multiple paths, but <cr>only one</c> can be <cr>active</c> at a time."
    );
    auto infoBtn = InfoAlertButton::create(infoTitle->getCString(), infoDesc->getCString(), 0.8f);
    m_buttonMenu->addChild(infoBtn);
    infoBtn->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(
        bgWidth * 0.5f + centerX - 14.0f,
        bgHeight * 0.5f + centerY - 14.0f
    )));
    
    auto pathSprite = GJPathSprite::create(path);
    pathSprite->updateState();
    m_mainLayer->addChild(pathSprite, 1);
    pathSprite->setScale(1.0f);
    
    auto labelFrame = cocos2d::CCString::createWithFormat("pathLabel_%02d_001.png", path);
    CCSpriteGrayscale* labelSprite;
    if (isUnlocked) {
        labelSprite = static_cast<CCSpriteGrayscale*>(cocos2d::CCSprite::createWithSpriteFrameName(labelFrame->getCString()));
    } else {
        labelSprite = CCSpriteGrayscale::createWithSpriteFrameName(labelFrame->getCString());
        float luminance = (path >= 7) ? 3.0f : 1.0f;
        labelSprite->setEvenLuminance(luminance);
        labelSprite->setColor(cocos2d::ccc3(100, 100, 100));
    }
    m_mainLayer->addChild(labelSprite, 1);
    
    cocos2d::CCPoint labelPos(centerX, bgHeight * 0.5f + centerY - 8.0f);
    float pathWidth = pathSprite->getContentSize().width;
    float labelWidth = labelSprite->getContentSize().width;
    float totalWidth = pathWidth + labelWidth + 10.0f;
    
    pathSprite->setPosition(labelPos + cocos2d::CCPoint(-(pathWidth * 0.5f - totalWidth * 0.5f), 0.0f));
    labelSprite->setPosition(labelPos + cocos2d::CCPoint(totalWidth * -0.5f + labelWidth * 0.5f, -2.0f));
    
    auto progressBg = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");
    progressBg->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    progressBg->setPosition(cocos2d::CCPoint(centerX - 170.0f, 30.0f - (bgHeight * 0.5f - centerY) + 10.0f));
    m_mainLayer->addChild(progressBg, 0);
    progressBg->setScaleX(340.0f / progressBg->getContentSize().width);
    progressBg->setScaleY(5.0f / progressBg->getContentSize().height);
    progressBg->setColor(progressColor);
    
    int currentProgress = stats->getStatFromKey(static_cast<StatKey>(path + 29));
    float progress = std::min(static_cast<float>(currentProgress) / 1000.0f, 1.0f);
    
    if (currentProgress > 999) {
        currentProgress = 1000;
        progress = 1.0f;
    }
    
    if (progress > 0.0f) {
        auto progressFill = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare20_001.png");
        progressFill->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
        progressFill->setPosition(progressBg->getPosition() + cocos2d::CCPoint(2.0f, 0.0f));
        m_mainLayer->addChild(progressFill, 1);
        progressFill->setScaleX((progress * 336.0f) / progressFill->getContentSize().width);
        progressFill->setScaleY(2.0f / progressFill->getContentSize().height);
        progressFill->setColor(bgColor);
    }
    
    for (int i = 0; i < 11; ++i) {
        auto key = cocos2d::CCString::createWithFormat("geometry.ach.path%02d.%02d", path, i);
        int unlockId = 0;
        UnlockType unlockType = UnlockType::Unknown;
        GameManager::sharedState()->getUnlockForAchievement(key->getCString(), &unlockId, &unlockType);
        
        if (unlockType == UnlockType::Unknown) {
            unlockType = UnlockType::Type1;
            unlockId = 1;
        }
        
        auto icon = GJItemIcon::create(unlockType, unlockId, cocos2d::ccc3(0, 0, 0), cocos2d::ccc3(255, 255, 255), 0, true, true, cocos2d::ccc3(255, 255, 255));
        if (icon) {
            icon->setScale(GJItemIcon::scaleForType(UnlockType::Type1));
            
            auto iconItem = CCMenuItemSpriteExtra::create(icon, this, menu_selector(GJPathPage::onIconInfo));
            m_buttonMenu->addChild(iconItem);
            iconItem->setContentSize(icon->getContentSize());
            icon->setPosition(cocos2d::CCPoint(iconItem->getContentSize().width * 0.5f, iconItem->getContentSize().height * 0.5f));
            iconItem->setTag(i);
            
            bool earned = false;
            if (i == 0 && isUnlocked) {
                earned = true;
            } else {
                earned = AchievementManager::sharedState()->isAchievementEarned(key->getCString());
            }
            
            if (!earned) {
                icon->changeToLockedState(1.0f / icon->getScale());
                icon->darkenStoreItem(cocos2d::ccc3(0, 0, 0));
            }
            
            float rowY = (i & 1) ? centerY : centerY;
            iconItem->setPosition(m_buttonMenu->convertToNodeSpace(
                m_mainLayer->convertToWorldSpace(cocos2d::CCPoint(
                    centerX - 170.0f + i * 34.0f,
                    rowY + 10.0f
                ))
            ));
            
            auto dot = cocos2d::CCSprite::create("smallDot.png");
            dot->setAnchorPoint(cocos2d::CCPoint(0.5f, 1.0f));
            dot->setPosition(cocos2d::CCPoint(centerX - 170.0f + i * 34.0f, rowY - 17.0f + 10.0f));
            dot->setScaleX(1.5f / dot->getContentSize().width);
            dot->setScaleY(16.0f / dot->getContentSize().height);
            dot->setColor(cocos2d::ccc3(125, 125, 125));
            dot->setOpacity(25);
            m_mainLayer->addChild(dot, 0);
            
            auto dotSmall = cocos2d::CCSprite::create("smallDot.png");
            dotSmall->setPosition(cocos2d::CCPoint(centerX - 170.0f + i * 34.0f, progressBg->getPosition().y));
            dotSmall->setColor(progressColor);
            m_mainLayer->addChild(dotSmall, 0);
            
            if (earned) {
                auto check = cocos2d::CCSprite::create("smallDot.png");
                check->setPosition(cocos2d::CCPoint(centerX - 170.0f + i * 34.0f, progressBg->getPosition().y));
                check->setColor(bgColor);
                check->setScale(0.7f);
                m_mainLayer->addChild(check, 1);
            }
        }
    }
    
    if (isUnlocked) {
        auto label = cocos2d::CCLabelBMFont::create(
            cocos2d::CCString::createWithFormat("%i/%i", currentProgress, 1000)->getCString(),
            "bigFont.fnt"
        );
        label->setScale(0.6f);
        m_mainLayer->addChild(label, 1);
        label->setPosition(cocos2d::CCPoint(centerX - 12.0f, 12.0f - (bgHeight * 0.5f - centerY) + 7.0f));
        
        auto starIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
        m_mainLayer->addChild(starIcon, 1);
        starIcon->setPosition(label->getPosition() + cocos2d::CCPoint(label->getContentSize().width * label->getScale() * 0.5f + 7.0f, -1.5f));
        starIcon->setScale(0.5f);
        
        auto moonIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_moonsIcon_001.png");
        m_mainLayer->addChild(moonIcon, 1);
        moonIcon->setPosition(starIcon->getPosition() + cocos2d::CCPoint(12.0f, 0.0f));
        moonIcon->setScale(0.55f);
        
        if (progress < 1.0f) {
            int activePath = stats->getActivePath();
            bool isActive = (path + 29) == activePath;
            
            GameToolbox::createToggleButton(
                "Active",
                menu_selector(GJPathPage::onActivatePath),
                0,
                isActive,
                m_buttonMenu,
                cocos2d::CCPoint(17.0f - (bgWidth * 0.5f - centerX), -(bgHeight * 0.5f - centerY) - 8.0f),
                this,
                m_mainLayer,
                0.6f,
                0.5f,
                100.0f,
                cocos2d::CCPoint(6.0f, 0.5f),
                "goldFont.fnt",
                false,
                false,
                0
            );
        }
        
        auto chestIcon = cocos2d::CCSprite::createWithSpriteFrameName("chestIcon_001.png");
        m_mainLayer->addChild(chestIcon, 3);
        chestIcon->setColor(progressColor);
        chestIcon->setPosition(cocos2d::CCPoint(bgWidth * 0.5f + centerX - 28.5f, progressBg->getPosition().y));
        chestIcon->setOpacity(200);
    } else {
        auto lockSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_lock_001.png");
        lockSprite->setScale(1.2f);
        auto lockItem = CCMenuItemSpriteExtra::create(lockSprite, this, menu_selector(GJPathPage::onUnlock));
        m_buttonMenu->addChild(lockItem);
        lockItem->setPosition(m_buttonMenu->convertToNodeSpace(cocos2d::CCPoint(centerX, 15.0f - (bgHeight * 0.5f - centerY))));
        
        int price = 0;
        // Get price from store item
        auto storeItem = stats->getStoreItem(path + 5);
        if (storeItem) {
            price = storeItem->getPrice();
        }
        
        auto priceLabel = cocos2d::CCLabelBMFont::create(GameToolbox::pointsToString(price).c_str(), "bigFont.fnt");
        lockSprite->addChild(priceLabel);
        priceLabel->setPosition(cocos2d::CCPoint(lockSprite->getContentSize().width * 0.5f - 4.0f, -6.5f));
        priceLabel->setScale(0.35f);
        
        auto orbIcon = cocos2d::CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
        lockSprite->addChild(orbIcon);
        orbIcon->setPosition(priceLabel->getPosition() + cocos2d::CCPoint(priceLabel->getContentSize().width * priceLabel->getScale() * 0.5f + 6.0f + (path == 4 ? 1.0f : 0.0f), -0.5f));
        orbIcon->setScale(0.5f);
    }
    
    return true;
}

void GJPathPage::keyBackClicked() {
    onBack(nullptr);
}

void GJPathPage::onActivatePath(cocos2d::CCObject* sender) {
    GameStatsManager::sharedState()->updateActivePath(static_cast<StatKey>(m_pathNumber + 29));
}

void GJPathPage::onBack(cocos2d::CCObject* sender) {
    if (m_animationPlaying) {
        return;
    }
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
    
    stopAllActions();
    unscheduleAllSelectors();
    setKeypadEnabled(false);
    removeFromParentAndCleanup(true);
    GJPathsLayer::darkenButtons(m_pathsLayer, false);
}

void GJPathPage::onIconInfo(cocos2d::CCObject* sender) {
    int index = sender->getTag();
    auto key = cocos2d::CCString::createWithFormat("geometry.ach.path%02d.%02d", m_pathNumber, index);
    
    int unlockId = 0;
    UnlockType unlockType = UnlockType::Unknown;
    GameManager::sharedState()->getUnlockForAchievement(key->getCString(), &unlockId, &unlockType);
    
    if (unlockType != UnlockType::Unknown) {
        auto popup = ItemInfoPopup::create(unlockId, unlockType);
        popup->show();
    }
}

void GJPathPage::onUnlock(cocos2d::CCObject* sender) {
    auto stats = GameStatsManager::sharedState();
    auto storeItem = stats->getStoreItem(m_pathNumber + 5);
    
    if (!storeItem) {
        return;
    }
    
    int price = storeItem->getPrice();
    auto currencyKey = storeItem->getCurrencyKey();
    int currency = stats->getStat(currencyKey.c_str());
    
    if (currency < price) {
        showCantAffordMessage(storeItem);
    } else {
        auto popup = PurchaseItemPopup::create(storeItem);
        popup->m_delegate = this;
        popup->show();
    }
}

void GJPathPage::showCantAffordMessage(GJStoreItem* item) {
    std::string text;
    const char* currencyName = (item->m_shopType == ShopType::Diamond) ? "<cc>Diamond Shards</c>" : "<cl>Mana Orbs</c>";
    
    switch (m_dialogIndex) {
        case 0:
            text = "You cannot <cg>afford</c> that <cl>path</c>.";
            break;
        case 1:
            text = std::string("<cg>Collect</c> more ") + currencyName + ".";
            break;
        case 2:
            text = std::string("You <co>do not</c> have enough ") + currencyName + ".";
            break;
        default:
            m_dialogIndex = 0;
            text = "You cannot <cg>afford</c> that <cl>path</c>.";
            break;
    }
    
    auto dialogObj = DialogObject::create("The Shopkeeper", text, 5, 1.0f, 0, cocos2d::ccc3(255, 255, 255));
    auto dialog = DialogLayer::create(dialogObj, 2);
    addChild(dialog, 100);
    dialog->updateChatPlacement(DialogPlacement::Center);
    dialog->animateIn(DialogAnimation::FromCenter);
    
    ++m_dialogIndex;
}

void GJPathPage::show() {
    if (!m_noElasticity) {
        unsigned char opacity = getOpacity();
        cocos2d::CCPoint pos = m_mainLayer->getPosition();
        
        m_mainLayer->setPosition(cocos2d::CCPoint(-160.0f, pos.y));
        
        auto move = cocos2d::CCMoveTo::create(0.5f, pos);
        auto ease = cocos2d::CCEaseElasticOut::create(move, 0.6f);
        m_mainLayer->runAction(ease);
        
        auto director = cocos2d::CCDirector::sharedDirector();
        auto scene = director->getRunningScene();
        
        if (!m_scene) {
            m_scene = scene;
            int highestZ = scene->getHighestChildZ();
            m_ZOrder = (highestZ > 104) ? highestZ + 1 : 105;
        }
        
        scene->addChild(this, m_ZOrder);
        setOpacity(0);
        runAction(cocos2d::CCFadeTo::create(0.14f, opacity));
        setVisible(true);
    } else {
        FLAlertLayer::show();
    }
}

void GJPathPage::registerWithTouchDispatcher() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void GJPathPage::playUnlockAnimation() {
    m_animationPlaying = true;
    
    auto audio = FMODAudioEngine::sharedEngine();
    audio->fadeOutMusic(0.5f, 0);
    audio->playEffect("unlockPath.ogg", 0.0f, 0.0f, 0.6f);
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    
    auto overlay = cocos2d::CCLayerColor::create(cocos2d::ccc4(0, 0, 0, 0));
    addChild(overlay, 10);
    overlay->runAction(cocos2d::CCFadeTo::create(1.0f, 175));
    
    auto pathSprite = GJPathSprite::create(m_pathNumber);
    pathSprite->changeToLockedArt();
    addChild(pathSprite, 12);
    pathSprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    pathSprite->setScale(0.01f);
    
    auto scaleUp = cocos2d::CCScaleTo::create(2.0f, 2.0f);
    auto easeScale = cocos2d::CCEaseInOut::create(scaleUp, 1.0f);
    pathSprite->runAction(easeScale);
    
    auto actions = cocos2d::CCArray::create();
    actions->addObject(easeScale);
    actions->addObject(cocos2d::CCDelayTime::create(1.0f));
    
    for (int i = 0; i < 60; ++i) {
        float magnitude = static_cast<float>(i + 1) * 0.1f;
        float dx = (rand() * 4.6566e-10f * 2.0f - 1.0f) * magnitude;
        float dy = (rand() * 4.6566e-10f * 2.0f - 1.0f) * magnitude;
        cocos2d::CCPoint pos = pathSprite->getPosition() + cocos2d::CCPoint(dx, dy);
        actions->addObject(cocos2d::CCMoveTo::create(0.01f, pos));
        actions->addObject(cocos2d::CCDelayTime::create(0.016667f));
    }
    
    actions->addObject(cocos2d::CCMoveTo::create(0.01f, pathSprite->getPosition()));
    pathSprite->runAction(cocos2d::CCSequence::create(actions));
    
    auto particleStr = "84a1.05a1.25a0.16a50a180a180a29a0a11a0a0a0a0a0a0a0a2a1a0a0a1a0a0a0a0.360784a0a1a0a1a1a0a0a1a0a0a0a0a0a1a0a0.8a0a0a0a110a0a0a0a0a0a1a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
    auto particles = GameToolbox::particleFromString(particleStr, nullptr, nullptr);
    particles->setAutoRemoveOnFinish(true);
    addChild(particles, 11);
    particles->setScale(2.0f);
    particles->resetSystem();
    particles->setPosition(pathSprite->getPosition());
    particles->setStartColor(lightColorForPath(m_pathNumber));
    particles->setEndColor(darkColorForPath(m_pathNumber));
    
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.5f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GJPathPage::unlockAnimationStep2)),
        nullptr
    ));
    
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(2.4f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GJPathPage::unlockAnimationFinished)),
        nullptr
    ));
}

void GJPathPage::unlockAnimationStep2() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    
    auto particleStr = "84a0.97a1a0.16a72a180a180a29a0a11a0a0a0a0a0a0a0a2a1a0a0a1a0a0a0a0a0a1a0a1a1a0a0a1a0a0.568627a0a0a0a1a0a0.8a0a0.2a0a150a0a0a0a0a0a1a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
    auto particles = GameToolbox::particleFromString(particleStr, nullptr, nullptr);
    particles->setAutoRemoveOnFinish(true);
    addChild(particles, 11);
    particles->setScale(2.0f);
    particles->resetSystem();
    particles->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    particles->setStartColor(lightColorForPath(m_pathNumber));
    particles->setEndColor(darkColorForPath(m_pathNumber));
}

void GJPathPage::unlockAnimationStep3() {
    auto overlay = cocos2d::CCLayerColor::create(cocos2d::ccc4(0, 0, 0, 0));
    addChild(overlay, 10);
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    
    auto pathSprite = GJPathSprite::create(m_pathNumber);
    pathSprite->addShardSprite();
    addChild(pathSprite, 13);
    pathSprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    pathSprite->setScale(2.0f);
    
    float currentScale = pathSprite->getScale();
    auto scaleBounce1 = cocos2d::CCEaseOut::create(cocos2d::CCScaleTo::create(0.2f, currentScale + 1.0f), 2.0f);
    auto scaleBounce2 = cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(0.75f, 1.0f), 2.0f);
    auto delay = cocos2d::CCDelayTime::create(0.4f);
    auto scaleShrink = cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(2.5f, 0.2f), 2.0f);
    pathSprite->runAction(cocos2d::CCSequence::create(scaleBounce1, scaleBounce2, delay, scaleShrink, nullptr));
    
    pathSprite->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(1.9f),
        cocos2d::CCFadeOut::create(1.5f),
        nullptr
    ));
    
    overlay->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(1.9f),
        cocos2d::CCFadeTo::create(1.5f, 0),
        nullptr
    ));
    
    auto particleStr = "50a0a0.88a0.21a-1a90a180a5a5a38a38a0a0a0a0a0a0a7a2a0a360a0.411765a0a1a0a0a0a1a0a3a2a0a60a0a0a0.737255a0a0.160784a0a1a0a0.16a0.1a0.51a0.12a0a0a0a0a0a0a0a2a1a1a0a0a166a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
    auto particles1 = GameToolbox::particleFromString(particleStr, nullptr, nullptr);
    particles1->setAutoRemoveOnFinish(true);
    addChild(particles1, 15);
    particles1->setScale(1.5f);
    particles1->resetSystem();
    particles1->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    particles1->setStartColor(lightColorForPath(m_pathNumber));
    particles1->setEndColor(darkColorForPath(m_pathNumber));
    
    auto particleStr2 = "5a0a1.15a0.3a-1a110a97a0a8a19a19a0a0a0a0a0a0a100a12a0a0a1a0a0.890196a0a0a0a0.51a0.22a-5a1a0a0a1a0a0a0a0.0784314a0a0a0a0.16a0a0.25a0a0a0a0a0a0a0a0a2a0a0a0a0a159a0a0.35a0a0a0a0a1a0a0a0a0a0a0a0";
    auto particles2 = GameToolbox::particleFromString(particleStr2, nullptr, nullptr);
    particles2->setAutoRemoveOnFinish(true);
    addChild(particles2, 16);
    particles2->setScale(2.2f);
    particles2->resetSystem();
    particles2->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));
    particles2->setPosVar(cocos2d::CCPoint(5.0f, 5.0f));
    
    auto key = cocos2d::CCString::createWithFormat("geometry.ach.path%02d.00", m_pathNumber);
    GameManager::sharedState()->reportAchievementWithID(key->getCString(), 100, false);
    
    auto audio = FMODAudioEngine::sharedEngine();
    audio->fadeInMusic(4.0f, 0);
}

void GJPathPage::unlockAnimationFinished() {
    m_animationPlaying = false;
    
    int pathKey = m_pathNumber + 29;
    auto stats = GameStatsManager::sharedState();
    stats->updateActivePath(static_cast<StatKey>(pathKey));
    
    onBack(nullptr);
    
    m_pathsLayer->m_exiting = false;
    m_pathsLayer->onClose(nullptr);
    
    auto newLayer = GJPathsLayer::create();
    newLayer->m_noElasticity = true;
    newLayer->show();
    
    auto newPage = GJPathPage::create(m_pathNumber, newLayer);
    newPage->m_noElasticity = true;
    newPage->show();
    newPage->unlockAnimationStep3();
}

GJPathPage::~GJPathPage() {
}