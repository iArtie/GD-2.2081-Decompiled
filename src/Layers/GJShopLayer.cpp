#include "Layers/GJShopLayer.h"
#include "Cocos/CCCounterLabel.h"
#include "UI/AnimatedShopKeeper.h"
#include "Models/GJStoreItem.h"
#include "UI/GJItemIcon.h"
#include "Managers/GameStatsManager.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Layers/DialogLayer.h"
#include "Models/DialogObject.h"
#include "Editor/ListButtonBar.h"
#include "Layers/BoomScrollLayer.h"
#include "Popups/PurchaseItemPopup.h"
#include "Layers/RewardUnlockLayer.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Audio/FMODAudioEngine.h"
#include "Cocos/CCSpriteWithHue.h"
#include "Cocos/CCSpriteGrayscale.h"
#include "Utils/AdToolbox.h"

GJShopLayer* GJShopLayer::create(ShopType type) {
    auto layer = new GJShopLayer();
    if (layer && layer->init(type)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool GJShopLayer::init(ShopType type) {
    if (!CCLayer::init()) {
        return false;
    }
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->registerForcePrio(this, 2);
    
    m_type = type;
    m_closing = false;
    m_videoPlaying = false;
    m_zolgurothDialogIndex = 0;
    m_affordDialogIndex = 0;
    
    std::string bgFrame = "shopBG_001.png";
    std::string deskFrame = "storeDesk_001.png";
    std::string signFrame = "shopSign_001.png";
    std::string musicFile = "shop.mp3";
    std::string sheetName = "GJ_ShopSheet01";
    
    switch (m_type) {
        case ShopType::Secret:
            signFrame = "shopSign2_001.png";
            musicFile = "secretShop.mp3";
            break;
        case ShopType::Community:
            signFrame = "shopSign3_001.png";
            musicFile = "shop3.ogg";
            break;
        case ShopType::Mechanic:
            sheetName = "GJ_ShopSheet02";
            bgFrame = "shopBG_02_001.png";
            deskFrame = "storeDesk_02_001.png";
            signFrame = "shopSign4_001.png";
            musicFile = "shop4.mp3";
            break;
        case ShopType::Diamond:
            sheetName = "GJ_ShopSheet03";
            bgFrame = "shopBG_03_001.png";
            deskFrame = "storeDesk_03_001.png";
            signFrame = "shopSign5_001.png";
            musicFile = "shop5.ogg";
            break;
        default:
            break;
    }
    
    m_sheetName = sheetName;
    
    auto textureCache = cocos2d::CCTextureCache::sharedTextureCache();
    textureCache->addImage((sheetName + ".png").c_str());
    
    auto frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    frameCache->addSpriteFramesWithFile((sheetName + ".plist").c_str());
    
    setKeypadEnabled(true);
    setKeyboardEnabled(true);
    setTouchEnabled(true);
    setMouseEnabled(true);
    
    auto winSize = director->getWinSize();
    
    CCSpriteWithHue* bgSprite;
    if (m_type == ShopType::Community) {
        bgSprite = CCSpriteWithHue::createWithSpriteFrameName(bgFrame.c_str());
        bgSprite->setHueDegrees(-110.0f);
        bgSprite->setColor(cocos2d::ccc3(240, 240, 240));
    } else {
        bgSprite = static_cast<CCSpriteWithHue*>(cocos2d::CCSprite::createWithSpriteFrameName(bgFrame.c_str()));
    }
    
    bgSprite->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    addChild(bgSprite, -2);
    bgSprite->setScaleX((winSize.width + 5.0f) / bgSprite->getTextureRect().size.width);
    bgSprite->setScaleY((winSize.height + 5.0f) / bgSprite->getTextureRect().size.height);
    bgSprite->setPosition(cocos2d::CCPoint(-2.5f, -2.5f));
    
    auto sign = cocos2d::CCSprite::createWithSpriteFrameName(signFrame.c_str());
    sign->setPosition(cocos2d::CCPoint(winSize.width * 0.5f - 70.0f, winSize.height - 38.0f));
    addChild(sign);
    
    m_shopKeeper = AnimatedShopKeeper::create(type);
    addChild(m_shopKeeper, 3);
    m_shopKeeper->setPosition(cocos2d::CCPoint(winSize.width * 0.5f + 100.0f, 230.0f));
    m_shopKeeper->runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.1f),
        cocos2d::CCCallFunc::create(m_shopKeeper, callfunc_selector(AnimatedShopKeeper::startAnimating)),
        nullptr
    ));
    
    CCSpriteWithHue* deskSprite;
    if (m_type == ShopType::Community) {
        deskSprite = CCSpriteWithHue::createWithSpriteFrameName(deskFrame.c_str());
        deskSprite->setHueDegrees(-120.0f);
    } else {
        deskSprite = static_cast<CCSpriteWithHue*>(cocos2d::CCSprite::createWithSpriteFrameName(deskFrame.c_str()));
    }
    
    addChild(deskSprite, 1);
    deskSprite->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, 95.0f));
    deskSprite->setScaleX(0.96f);
    
    if (m_type == ShopType::Secret) {
        bgSprite->setColor(cocos2d::ccc3(0, 255, 255));
        deskSprite->setColor(cocos2d::ccc3(0, 255, 255));
    } else if (m_type == ShopType::Mechanic) {
        bgSprite->setColor(cocos2d::ccc3(175, 175, 175));
    } else if (m_type == ShopType::Community) {
        auto menu = cocos2d::CCMenu::create();
        addChild(menu, -1);
        menu->setPosition(cocos2d::CCPoint(winSize.width - 40.0f, winSize.height - 60.0f));
        
        auto creditsBtn = cocos2d::CCSprite::createWithSpriteFrameName("communityCreditsBtn_001.png");
        auto creditsItem = CCMenuItemSpriteExtra::create(creditsBtn, this, menu_selector(GJShopLayer::onCommunityCredits));
        creditsItem->setSizeMult(1.2f);
        menu->addChild(creditsItem);
    }
    
    if (m_type == ShopType::Main) {
        auto plushieSprite = cocos2d::CCSprite::createWithSpriteFrameName("plushies_001.png");
        plushieSprite->setScale(1.1f);
        auto plushieItem = CCMenuItemSpriteExtra::create(plushieSprite, this, menu_selector(GJShopLayer::onPlushies));
        plushieItem->m_animationEnabled = false;
        plushieItem->m_colorEnabled = true;
        auto plushieMenu = cocos2d::CCMenu::createWithItem(plushieItem);
        addChild(plushieMenu, 0);
        plushieMenu->setPosition(deskSprite->getPosition() + cocos2d::CCPoint(-130.0f, 120.0f));
        plushieMenu->setEnabled(false);
    }
    
    auto backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    auto backItem = CCMenuItemSpriteExtra::create(backSprite, this, menu_selector(GJShopLayer::onBack));
    backItem->setSizeMult(1.6f);
    auto backMenu = cocos2d::CCMenu::create(backItem, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(cocos2d::CCPoint(
        director->getScreenLeft() + 24.0f,
        director->getScreenTop() - 23.0f
    ));
    
    if (PlatformToolbox::isControllerConnected()) {
        GameToolbox::addBackButton(this, backItem);
    }
    
    auto audio = FMODAudioEngine::sharedEngine();
    audio->playMusic(musicFile.c_str(), true, 0.0f, false);
    
    int currency = 0;
    if (m_type == ShopType::Diamond) {
        currency = GameStatsManager::sharedState()->getStat("29");
    } else {
        currency = GameStatsManager::sharedState()->getStat("14");
    }
    
    m_currencyLabel = CCCounterLabel::create(currency, "bigFont.fnt", LabelType::Integer);
    addChild(m_currencyLabel, 3);
    m_currencyLabel->setPosition(cocos2d::CCPoint(winSize.width - 34.0f, winSize.height - 15.0f));
    m_currencyLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_currencyLabel->setScale(0.6f);
    
    const char* currencyIcon = (m_type == ShopType::Diamond) ? "currencyDiamondIcon_001.png" : "currencyOrbIcon_001.png";
    auto icon = cocos2d::CCSprite::createWithSpriteFrameName(currencyIcon);
    addChild(icon, 3);
    icon->setPosition(m_currencyLabel->getPosition() + cocos2d::CCPoint(13.0f, -0.5f));
    
    auto particles = cocos2d::CCParticleSystemQuad::create("keyEffect.plist");
    particles->setTotalParticles(8);
    particles->setStartColor(cocos2d::ccColor4F(1.0f, 0.78f, 0.0f, 1.0f));
    particles->setEndColor(cocos2d::ccColor4F(1.0f, 0.59f, 0.0f, 1.0f));
    particles->setStartColorVar(cocos2d::ccColor4F(0.0f, 0.0f, 0.0f, 0.0f));
    particles->setEndColorVar(cocos2d::ccColor4F(0.0f, 0.0f, 0.0f, 0.0f));
    particles->setLife(1.0f);
    particles->setLifeVar(0.2f);
    particles->setPosVar(cocos2d::CCPoint(12.0f, 10.0f));
    addChild(particles, 100);
    particles->setPosition(icon->getPosition());
    particles->setPositionType(cocos2d::kCCPositionTypeGrouped);
    particles->resetSystem();
    
    auto items = cocos2d::CCArray::create();
    auto storeItems = GameStatsManager::sharedState()->m_storeItemArray;
    auto filteredItems = cocos2d::CCArray::create();
    
    bool secretShopUnlocked = GameManager::sharedState()->getUGV("7");
    
    for (int i = 0; i < storeItems->count(); ++i) {
        auto item = storeItems->objectAtIndex(i);
        if (item->m_shopType == static_cast<int>(m_type)) {
            if (!secretShopUnlocked && m_type == ShopType::Secret && item->m_itemID == 20) {
                continue;
            }
            filteredItems->addObject(item);
        }
    }
    
    m_shopItems = cocos2d::CCDictionary::create();
    m_shopItems->retain();
    
    auto placeholder = cocos2d::CCSprite::createWithSpriteFrameName("playerSquare_001.png");
    auto rowItems = cocos2d::CCArray::create();
    
    for (int i = 0; i < filteredItems->count(); ++i) {
        auto item = filteredItems->objectAtIndex(i);
        int itemId = item->m_itemID;
        UnlockType unlockType = static_cast<UnlockType>(item->m_unlockType);
        bool unlocked = GameStatsManager::sharedState()->isStoreItemUnlocked(itemId);
        
        auto icon = GJItemIcon::createStoreItem(unlockType, itemId, unlocked, GJItemIcon::unlockedColorForType(static_cast<int>(m_type)));
        icon->setScale(1.1f);
        
        auto selectItem = CCMenuItemSpriteExtra::create(icon, this, menu_selector(GJShopLayer::onSelectItem));
        selectItem->setSizeMult(2.0f);
        items->addObject(selectItem);
        selectItem->setTag(itemId);
        icon->setTag(static_cast<int>(unlockType));
        
        if (unlockType != UnlockType::Type8 && unlockType != UnlockType::Type9 && unlockType != UnlockType::Type1) {
            rowItems->addObject(selectItem);
        }
        
        if (unlocked) {
            selectItem->setEnabled(false);
            icon->darkenStoreItem(m_type);
        } else {
            auto priceLabel = cocos2d::CCLabelBMFont::create(GameToolbox::pointsToString(item->m_price).c_str(), "bigFont.fnt");
            icon->addChild(priceLabel);
            priceLabel->setPosition(cocos2d::CCPoint(icon->getContentSize().width * 0.5f - 4.0f, -6.5f));
            priceLabel->setScale(0.35f);
            priceLabel->setTag(1234);
            
            const char* priceIcon = (m_type == ShopType::Diamond) ? "currencyDiamondIcon_001.png" : "currencyOrbIcon_001.png";
            auto priceIconSprite = cocos2d::CCSprite::createWithSpriteFrameName(priceIcon);
            icon->addChild(priceIconSprite);
            priceIconSprite->setPosition(priceLabel->getPosition() + cocos2d::CCPoint(priceLabel->getContentSize().width * priceLabel->getScale() * 0.5f + 6.0f + (m_type == ShopType::Diamond ? 1.0f : 0.0f), -0.5f));
            priceIconSprite->setScale(0.5f);
            priceIconSprite->setTag(1234);
        }
        
        m_shopItems->setObject(selectItem, itemId);
    }
    
    auto listBar = ListButtonBar::create(items, cocos2d::CCPoint(25.0f, 15.0f), cocos2d::CCPoint(winSize.width * 0.5f, 93.0f), 4, 60.0f, 221.0f, 0.0f, 0.0f, 2);
    addChild(listBar, 100);
    
    auto scrollLayer = listBar->m_scrollLayer;
    listBar->m_useMoveAnimation = true;
    scrollLayer->m_unkFloat2 = 0.6f;
    scrollLayer->setTouchEnabled(true);
    listBar->goToPage(0);
    scrollLayer->m_doVisit = true;
    scrollLayer->m_clippingRect = cocos2d::CCRect(
        winSize.width * 0.5f - 203.0f,
        winSize.height * 0.5f - 100.0f,
        406.0f,
        200.0f
    );
    
    return true;
}

bool GJShopLayer::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_type != ShopType::Main) {
        return false;
    }
    
    auto location = touch->getLocation();
    auto local = convertToNodeSpace(location);
    
    auto shopPos = m_shopKeeper->getPosition();
    cocos2d::CCRect touchRect(
        shopPos.x - 40.0f,
        shopPos.y - 45.0f + 10.0f,
        80.0f,
        90.0f
    );
    
    if (!touchRect.containsPoint(local) || m_shopKeeper->m_reacting) {
        return true;
    }
    
    bool shouldReact = false;
    if (GameManager::sharedState()->getUGV("31")) {
        shouldReact = true;
    } else {
        auto stats = GameStatsManager::sharedState();
        bool chestUnlocked = stats->isSpecialChestUnlocked("0022");
        shouldReact = !chestUnlocked || m_shopKeeper->m_reactCount > 4;
    }
    
    if (shouldReact) {
        m_shopKeeper->m_reactCount = 2;
        showReactMessage();
        if (m_zolgurothDialogIndex == 9) {
            m_shopKeeper->m_reactCount = 0;
        }
        return true;
    }
    
    m_shopKeeper->playReactAnimation();
    return true;
}

void GJShopLayer::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    ccTouchEnded(touch, event);
}

void GJShopLayer::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
}

void GJShopLayer::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
}

void GJShopLayer::keyBackClicked() {
    onBack(nullptr);
}

void GJShopLayer::onBack(cocos2d::CCObject* sender) {
    if (m_closing) {
        return;
    }
    
    m_closing = true;
    
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->unregisterForcePrio(this);
    
    retain();
    removeFromParent();
    
    cocos2d::CCScene* scene = nullptr;
    if (m_type != ShopType::Main) {
        scene = SecretRewardsLayer::scene(1);
    } else {
        scene = GJGarageLayer::scene();
    }
    
    if (scene) {
        director->replaceScene(scene);
        setKeypadEnabled(false);
        scene->addChild(this, 1000);
        release();
        
        auto winSize = director->getWinSize();
        auto move = cocos2d::CCMoveTo::create(0.3f, cocos2d::CCPoint(0.0f, winSize.height));
        auto ease = cocos2d::CCEaseIn::create(move, 2.0f);
        auto call = cocos2d::CCCallFunc::create(this, callfunc_selector(GJShopLayer::onExit));
        auto sequence = cocos2d::CCSequence::create(ease, call, nullptr);
        runAction(sequence);
        
        if (m_type != ShopType::Main) {
            GameManager::sharedState()->fadeInMusic("secretLoop.mp3");
        } else {
            GameManager::sharedState()->fadeInMenuMusic();
        }
    }
}

void GJShopLayer::onCommunityCredits(cocos2d::CCObject* sender) {
    auto page = CommunityCreditsPage::create();
    page->m_scene = this;
    page->show();
}

void GJShopLayer::onExit() {
    auto frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    frameCache->removeSpriteFramesFromFile((m_sheetName + ".plist").c_str());
    
    auto textureCache = cocos2d::CCTextureCache::sharedTextureCache();
    textureCache->removeTextureForKey((m_sheetName + ".png").c_str());
}

void GJShopLayer::onPlushies(cocos2d::CCObject* sender) {
}

void GJShopLayer::onSelectItem(cocos2d::CCObject* sender) {
    if (m_closing) {
        return;
    }
    
    int itemId = sender->getTag();
    auto stats = GameStatsManager::sharedState();
    
    if (stats->isStoreItemUnlocked(itemId)) {
        return;
    }
    
    auto storeItem = stats->getStoreItem(itemId);
    if (!storeItem) {
        return;
    }
    
    int price = storeItem->getPrice();
    int currency = stats->getStat(storeItem->getCurrencyKey().c_str());
    
    if (currency < price) {
        showCantAffordMessage(storeItem);
    } else {
        auto popup = PurchaseItemPopup::create(storeItem);
        popup->m_delegate = this;
        popup->show();
    }
}

void GJShopLayer::onVideoAd(cocos2d::CCObject* sender) {
    if (!m_videoPlaying && AdToolbox::hasCachedRewardedVideo()) {
        m_videoPlaying = true;
        GameManager::sharedState()->m_rewardedVideoDelegate = this;
        AdToolbox::showRewardedVideo();
    }
}

void GJShopLayer::dialogClosed(DialogLayer* layer) {
    int tag = layer->getTag();
    
    if (tag == 1) {
        auto audio = FMODAudioEngine::sharedEngine();
        audio->playEffect("door001.ogg", 1.0f, 0.0f, 0.8f);
        
        auto overlay = cocos2d::CCLayerColor::create(cocos2d::ccc4(0, 0, 0, 255));
        addChild(overlay, 1000);
        
        audio->stopAllMusic(true);
        
        auto director = cocos2d::CCDirector::sharedDirector();
        auto dispatcher = director->getTouchDispatcher();
        dispatcher->unregisterForcePrio(this);
        
        auto scene = GJGarageLayer::scene();
        auto transition = cocos2d::CCTransitionFade::create(1.0f, scene);
        director->replaceScene(transition);
    } else if (tag == 2) {
        auto stats = GameStatsManager::sharedState();
        auto reward = stats->unlockSpecialChest("0022");
        
        if (reward) {
            auto unlockLayer = RewardUnlockLayer::create(reward->m_rewardType, 0);
            addChild(unlockLayer, 100);
            unlockLayer->showCollectReward(reward);
        }
    }
}

void GJShopLayer::didPurchaseItem(GJStoreItem* item) {
    auto audio = FMODAudioEngine::sharedEngine();
    audio->playEffect("buyItem03.ogg", 1.0f, 0.0f, 1.0f);
    
    auto stats = GameStatsManager::sharedState();
    int currency = stats->getStat(item->getCurrencyKey().c_str());
    m_currencyLabel->setTargetCount(currency);
    
    auto itemObj = m_shopItems->objectForKey(item->m_itemId);
    if (itemObj) {
        auto icon = static_cast<GJItemIcon*>(itemObj->getNormalImage());
        icon->darkenStoreItem(m_type);
        
        auto children = icon->getChildren();
        if (children) {
            for (int i = 0; i < children->count(); ++i) {
                auto child = children->objectAtIndex(i);
                if (child->getTag() == 1234) {
                    child->setVisible(false);
                }
            }
        }
    }
    
    if (item->m_unlockType == 12 && item->m_typeId == 4) {
        auto dialogs = cocos2d::CCArray::create();
        
        dialogs->addObject(DialogObject::create("Scratch", "Ah the <cl>shiny</c> thing, excellent choice.", 10, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
        dialogs->addObject(DialogObject::create("Scratch", "I found it a while ago.<d030>\nYea<d010>.<d010>.<d010>. 'Found' is a good word...", 13, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
        dialogs->addObject(DialogObject::create("Scratch", "Not sure what it does.<d020> If anyone asks you didnt buy it here.", 11, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
        dialogs->addObject(DialogObject::create("Scratch", "I mean, <d040>buy it where?", 13, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
        dialogs->addObject(DialogObject::create("Scratch", "<d010>.<d010>.<d010>.", 8, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
        
        auto dialogLayer = DialogLayer::createWithObjects(dialogs, 2);
        addChild(dialogLayer, 100);
        dialogLayer->updateChatPlacement(DialogPlacement::Center);
        dialogLayer->animateInRandomSide();
    }
}

void GJShopLayer::exitVideoAdItems() {
    if (m_unkNode1 && m_unkNode1->isVisible()) {
        m_unkNode1->setVisible(false);
        m_unkNode1->setScale(0.0f);
        if (m_unkNode2) {
            m_unkNode2->setVisible(false);
        }
        m_shopKeeper->setPosition(m_shopKeeper->getPosition() + cocos2d::CCPoint(-50.0f, 0.0f));
    }
}

void GJShopLayer::registerWithTouchDispatcher() {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto dispatcher = director->getTouchDispatcher();
    dispatcher->addTargetedDelegate(this, -500, true);
}

void GJShopLayer::rewardedVideoFinished() {
    m_videoPlaying = false;
    runAction(cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(2.5f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(GJShopLayer::updateCurrencyCounter)),
        nullptr
    ));
}

cocos2d::CCScene* GJShopLayer::scene(ShopType type) {
    auto scene = cocos2d::CCScene::create();
    auto layer = GJShopLayer::create(type);
    scene->addChild(layer);
    return scene;
}

void GJShopLayer::showCantAffordMessage(GJStoreItem* item) {
    std::string text;
    const char* currencyName = (item->m_shopType == ShopType::Diamond) ? "<cc>Diamond Shards</c>" : "<cl>Mana Orbs</c>";
    
    switch (m_affordDialogIndex) {
        case 0:
            text = "You cannot <cg>afford</c> that <cl>item</c>.";
            break;
        case 1:
            text = std::string("<cg>Collect</c> more ") + currencyName + ".";
            break;
        case 2:
            text = std::string("You <co>do not</c> have enough ") + currencyName + ".";
            break;
        default:
            m_affordDialogIndex = 0;
            text = "You cannot <cg>afford</c> that <cl>item</c>.";
            break;
    }
    
    auto dialogObj = DialogObject::create("The Shopkeeper", text, 5, 1.0f, 0, cocos2d::ccc3(255, 255, 255));
    auto dialog = DialogLayer::create(dialogObj, 2);
    addChild(dialog, 100);
    dialog->updateChatPlacement(DialogPlacement::Center);
    dialog->animateIn(DialogAnimation::FromCenter);
    
    ++m_affordDialogIndex;
}

void GJShopLayer::showReactMessage() {
    auto dialogs = cocos2d::CCArray::create();
    auto stats = GameStatsManager::sharedState();
    
    if (GameManager::sharedState()->getUGV("31")) {
        bool chestUnlocked = stats->isSpecialChestUnlocked("0022");
        
        if (!chestUnlocked) {
            dialogs->addObject(DialogObject::create("The Shopkeeper", "Hey, Welcome back!", 5, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            dialogs->addObject(DialogObject::create("The Shopkeeper", "Sorry, I think I fell asleep earlier. <d030>Did I miss something?", 29, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            dialogs->addObject(DialogObject::create("The Shopkeeper", "<cl>Oh yea!</c> Someone left this <cg>package</c> here for you.", 5, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            dialogs->addObject(DialogObject::create("The Shopkeeper", "Who is<d030> <cr>%s</c>?", 29, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            dialogs->addObject(DialogObject::create("The Shopkeeper", "<cl>I really need to get that thing checked.<d030>.<d030>.</c>", 34, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            
            auto dialog = DialogLayer::createWithObjects(dialogs, 2);
            dialog->m_delegate = this;
            dialog->m_skippable = true;
            dialog->setTag(2);
            addChild(dialog, 100);
            dialog->updateChatPlacement(DialogPlacement::Center);
            dialog->animateInRandomSide();
            return;
        } else {
            text = "Can I help you?";
            intVal = 2;
            intVal2 = 29;
            goto createDialog;
        }
    }
    
    switch (m_zolgurothDialogIndex) {
        case 0:
            text = "What do you want?";
            goto createDialog;
        case 1:
            text = "Can you stop?";
            intVal = 2;
            intVal2 = 6;
            goto createDialog;
        case 2:
            text = "<cr>Stop</c> with the <cg>poking</c>!";
            intVal = 2;
            intVal2 = 30;
            goto createDialog;
        case 3:
            text = "You are getting on my <cl>nerves</c>.";
            intVal = 2;
            intVal2 = 31;
            goto createDialog;
        case 4:
            text = "I'm <co>warning</c> you!";
            intVal = 2;
            intVal2 = 30;
            goto createDialog;
        case 5:
            text = "<d010>.<d010>.<d010>";
            auto audio = FMODAudioEngine::sharedEngine();
            audio->playMusic("dangerLoop.mp3", true, 0.0f, false);
            dialogs->addObject(DialogObject::create("The Shopkeeper", "<cg>Some things are better left alone</c>.", 32, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            dialogs->addObject(DialogObject::create("The Shopkeeper", "Oh no... Not again.", 34, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            dialogs->addObject(DialogObject::create("The Shopkeeper", "Just... just <cr>STOP</c> tapping and it will be fine!", 36, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            break;
        case 6:
            text = "It's best if you stop doing that.";
            intVal = 2;
            intVal2 = 34;
            goto createDialog;
        case 7:
            text = "<cl>.<d030>.<d030>.<d030></c>";
            intVal = 2;
            intVal2 = 35;
            goto createDialog;
        case 8:
            text = "<cg>I feel, <d050>strange.<d030>.<d030>.<d030></c>";
            intVal = 4;
            intVal2 = 32;
            goto createDialog;
        case 9:
            dialogs->addObject(DialogObject::create("Zolguroth", "<cr>You dare touch a god?</c>", 33, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            dialogs->addObject(DialogObject::create("Zolguroth", "<cr>You will regret your actions here today.</c>", 33, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            
            std::string playerName = GameManager::sharedState()->m_playerName;
            if (playerName.empty()) {
                playerName = "...";
            }
            dialogs->addObject(DialogObject::create("Zolguroth", cocos2d::CCString::createWithFormat("<cr>I have big things planned\nfor you<d020></c> <cl>%s</c>.", playerName.c_str())->getCString(), 33, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            dialogs->addObject(DialogObject::create("Zolguroth", "<cr>Now.<d030>.<d030>.<d030> </c><i050><s260><cr>BEGONE!</c></s></i>", 33, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
            
            auto dialog = DialogLayer::createWithObjects(dialogs, 2);
            dialog->m_delegate = this;
            dialog->m_skippable = true;
            dialog->m_noRemove = true;
            dialog->setTag(1);
            addChild(dialog, 100);
            dialog->updateChatPlacement(DialogPlacement::Center);
            dialog->animateInRandomSide();
            
            GameManager::sharedState()->setUGV("31", true);
            ++m_zolgurothDialogIndex;
            return;
    }
    
createDialog:
    if (dialogs->count() == 0) {
        dialogs->addObject(DialogObject::create("The Shopkeeper", text, intVal2, 1.0f, 0, cocos2d::ccc3(255, 255, 255)));
    }
    
    auto dialog = DialogLayer::createWithObjects(dialogs, intVal);
    dialog->m_skippable = true;
    addChild(dialog, 100);
    dialog->updateChatPlacement(DialogPlacement::Center);
    dialog->animateInRandomSide();
    
    ++m_zolgurothDialogIndex;
}

void GJShopLayer::updateCurrencyCounter() {
    int currency = GameStatsManager::sharedState()->getStat("14");
    m_currencyLabel->setTargetCount(currency);
}

GJShopLayer::~GJShopLayer() {
    if (m_shopItems) {
        m_shopItems->release();
    }
}