#include "Layers/LevelAreaInnerLayer.h"
#include "Cocos/CCSpriteGrayscale.h"
#include "Cocos/CCURLObject.h"
#include "Models/DialogObject.h"
#include "Audio/FMODAudioEngine.h"
#include "Managers/LocalLevelManager.h"
#include "Utils/PlatformToolbox.h"
#include "Utils/GameToolbox.h"
#include "Layers/FLAlertLayer.h"

LevelAreaInnerLayer* LevelAreaInnerLayer::create(bool returning) {
    auto layer = new LevelAreaInnerLayer();
    if (layer && layer->init(returning)) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool LevelAreaInnerLayer::init(bool returning) {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    // Load tower assets
    auto textureCache = cocos2d::CCTextureCache::sharedTextureCache();
    textureCache->addImage("TowerSheet.png");

    auto spriteFrameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    spriteFrameCache->addSpriteFramesWithFile("TowerSheet.plist");

    // Back button
    auto backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    backSprite->setOpacity(0);
    auto backButton = CCMenuItemSpriteExtra::create(
        backSprite,
        nullptr,
        this,
        menu_selector(LevelAreaInnerLayer::onBack)
    );
    backButton->setSizeMult(1.6f);
    backSprite->runAction(cocos2d::CCFadeIn::create(0.3f));

    auto backMenu = cocos2d::CCMenu::create(backButton, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f
    );

    if (PlatformToolbox::isControllerConnected()) {
        GameToolbox::addBackButton(this, backButton);
    }

    // Main container node
    auto container = cocos2d::CCNode::create();
    addChild(container);

    // Background
    auto menu = cocos2d::CCMenu::create();
    auto bgSprite = cocos2d::CCSprite::createWithSpriteFrameName("towerBG_01_001.png");
    container->addChild(bgSprite, 1);
    bgSprite->setPosition(winSize.width * 0.5f, winSize.height * 0.5f);

    // Door positions
    cocos2d::CCPoint doorPositions[4] = {
        cocos2d::CCPoint(-95.5f, -89.25f),
        cocos2d::CCPoint(103.75f, -33.5f),
        cocos2d::CCPoint(-95.5f, 21.25f),
        cocos2d::CCPoint(57.5f, 75.5f)
    };

    int completedFloors = 0;

    // Create doors for each floor
    for (int floorIndex = 0; floorIndex < 4; ++floorIndex) {
        int levelID = floorIndex + 5001;
        auto levelManager = GameLevelManager::sharedState();
        auto mainLevel = levelManager->getMainLevel(levelID, true);

        auto statsManager = GameStatsManager::sharedState();
        bool hasCompleted = statsManager->hasCompletedMainLevel(levelID);
        bool previousCompleted = false;

        if (floorIndex > 0) {
            previousCompleted = statsManager->hasCompletedMainLevel(levelID - 1);
        }

        std::string spriteName = "towerDoor_locked_001.png";

        if (floorIndex == 0) {
            if (hasCompleted) {
                spriteName = "towerDoor_open_001.png";
                completedFloors++;
            } else {
                spriteName = "towerDoor_unlocked_001.png";
            }
        } else {
            if (hasCompleted) {
                spriteName = "towerDoor_open_001.png";
                completedFloors++;
            } else if (previousCompleted) {
                spriteName = "towerDoor_unlocked_001.png";
            }
        }

        auto doorSprite = cocos2d::CCSprite::createWithSpriteFrameName(spriteName.c_str());
        auto doorButton = CCMenuItemSpriteExtra::create(
            doorSprite,
            nullptr,
            this,
            menu_selector(LevelAreaInnerLayer::onDoor)
        );
        doorButton->setColorEnabled(true);
        doorButton->setAnimationEnabled(false);
        doorButton->setTag(levelID);

        if (hasCompleted || !previousCompleted) {
            // Gray out locked door
            cocos2d::ccColor3B grayColor = cocos2d::ccc3(128, 128, 128);
            doorSprite->setColor(grayColor);
            doorButton->setEnabled(false);
        }

        auto doorPos = doorPositions[floorIndex];
        doorButton->setPosition(menu->convertToNodeSpace(doorPos));
        menu->addChild(doorButton);

        // Add coins for completed levels
        if (hasCompleted) {
            cocos2d::CCPoint coinOffset = doorPos + cocos2d::CCPoint(0.0f, 32.0f);

            for (int coinIndex = 1; coinIndex <= 3; ++coinIndex) {
                std::string coinKey = GJGameLevel::getCoinKey(mainLevel, coinIndex);
                bool hasCoin = statsManager->hasSecretCoin(coinKey);

                const char* coinSpriteName = hasCoin ? "GJ_coinsIcon_001.png" : "GJ_coinsIcon_gray_001.png";
                auto coinSprite = cocos2d::CCSprite::createWithSpriteFrameName(coinSpriteName);

                if (hasCoin) {
                    coinSprite->setColor(cocos2d::ccc3(255, 255, 255));
                } else {
                    coinSprite->setColor(cocos2d::ccc3(224, 224, 224));
                }

                coinSprite->setScale(0.7f);
                container->addChild(coinSprite, 10);

                float yOffset = (coinIndex == 2) ? 8.0f : 0.0f;
                cocos2d::CCPoint coinPos = coinOffset + cocos2d::CCPoint((coinIndex - 2) * 20.0f, yOffset);
                coinSprite->setPosition(coinPos);
            }
        }
    }

    // Next floor button
    auto nextFloorSprite = cocos2d::CCSprite::createWithSpriteFrameName("towerLockedArrow_001.png");
    auto nextFloorButton = CCMenuItemSpriteExtra::create(
        nextFloorSprite,
        nullptr,
        this,
        menu_selector(LevelAreaInnerLayer::onNextFloor)
    );
    m_nextFloorButton = nextFloorButton;
    menu->addChild(nextFloorButton);
    nextFloorButton->setPosition(menu->convertToNodeSpace(cocos2d::CCPoint(-96.0f, 108.0f)));

    container->addChild(menu, 2);

    // Add torches
    for (int torchIndex = 1; torchIndex <= 2; ++torchIndex) {
        auto torchSprite = cocos2d::CCSprite::createWithSpriteFrameName("towerTorchInner_001.png");
        container->addChild(torchSprite, 4);

        float xPos = (torchIndex == 1) ? -195.0f : 195.0f;
        torchSprite->setPosition(xPos, 4.0f);

        auto torchPos = torchSprite->getPosition();
        LevelAreaLayer::addTorch(container, 1.0f, torchPos + cocos2d::CCPoint(0.0f, 33.0f), 2, 4, 1, 0);
    }

    // Scale animation if returning from play
    if (!returning) {
        container->setScale(0.2f);
        auto scaleTo = cocos2d::CCScaleTo::create(1.0f, 1.0f);
        auto easeOut = cocos2d::CCEaseExponentialOut::create(scaleTo);
        container->runAction(easeOut);
    }

    // Online vault button
    auto vaultSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_secretLock4_small_001.png");
    vaultSprite->setOpacity(0);
    auto delay = cocos2d::CCDelayTime::create(0.5f);
    auto fadeTo = cocos2d::CCFadeTo::create(2.0f, 200);
    vaultSprite->runAction(cocos2d::CCSequence::create(delay, fadeTo, nullptr));

    auto vaultButton = CCMenuItemSpriteExtra::create(
        vaultSprite,
        nullptr,
        this,
        menu_selector(LevelAreaInnerLayer::onOnlineVault)
    );
    backMenu->addChild(vaultButton);

    auto screenRight = cocos2d::CCDirector::sharedDirector()->getScreenRight();
    auto vaultPos = cocos2d::CCPoint(screenRight - 22.0f, 23.0f);
    vaultButton->setPosition(backMenu->convertToNodeSpace(vaultPos));

    // Floating animation for container
    container->setPosition(container->getPosition() + cocos2d::CCPoint(0.0f, -2.0f));
    auto moveUp = cocos2d::CCMoveBy::create(2.0f, cocos2d::CCPoint(0.0f, 4.0f));
    auto moveDown = cocos2d::CCMoveBy::create(2.0f, cocos2d::CCPoint(0.0f, -4.0f));
    auto easeIn = cocos2d::CCEaseInOut::create(moveUp, 2.0f);
    auto easeOut = cocos2d::CCEaseInOut::create(moveDown, 2.0f);
    auto sequence = cocos2d::CCSequence::create(easeIn, easeOut, nullptr);
    container->runAction(cocos2d::CCRepeatForever::create(sequence));

    // Resume tower music
    auto resumeAction = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(LevelAreaInnerLayer::tryResumeTowerMusic)),
        nullptr
    );
    runAction(resumeAction);

    // Info button
    auto infoSprite = CCSpriteGrayscale::createWithSpriteFrameName("GJ_infoIcon_001.png");
    infoSprite->setOpacity(0);
    infoSprite->setScale(0.8f);

    auto infoDelay = cocos2d::CCDelayTime::create(0.1f);
    auto infoFade = cocos2d::CCFadeTo::create(1.0f, 200);
    infoSprite->runAction(cocos2d::CCSequence::create(infoDelay, infoFade, nullptr));

    auto infoButton = CCMenuItemSpriteExtra::create(
        infoSprite,
        nullptr,
        this,
        menu_selector(LevelAreaInnerLayer::onInfo)
    );
    backMenu->addChild(infoButton);
    infoButton->setSizeMult(1.2f);

    auto infoX = cocos2d::CCDirector::sharedDirector()->getScreenRight() - 20.0f;
    auto infoY = cocos2d::CCDirector::sharedDirector()->getScreenTop() - 20.0f;
    infoButton->setPosition(backMenu->convertToNodeSpace(cocos2d::CCPoint(infoX, infoY)));

    // Show floor 1 completion dialog if first time
    if (completedFloors == 4) {
        auto gameManager = GameManager::sharedState();
        if (!gameManager->getUGV("33")) {
            m_enteringLevel = true;
            auto dialogDelay = cocos2d::CCDelayTime::create(1.0f);
            auto showDialog = cocos2d::CCCallFunc::create(this, callfunc_selector(LevelAreaInnerLayer::showFloor1CompleteDialog));
            runAction(cocos2d::CCSequence::create(dialogDelay, showDialog, nullptr));
        }
    }

    if (completedFloors < 4) {
        m_nextFloorButton->setEnabled(false);
        m_nextFloorButton->setVisible(false);
    }

    if (m_enteringLevel) {
        GameManager::sharedState()->m_somethingInMenuLayer = 0;
    }

    return true;
}

void LevelAreaInnerLayer::keyBackClicked() {
    onBack(nullptr);
}

void LevelAreaInnerLayer::onBack(cocos2d::CCObject* sender) {
    if (!m_enteringLevel) {
        setKeypadEnabled(false);
        auto director = cocos2d::CCDirector::sharedDirector();
        auto scene = LevelAreaLayer::scene();
        auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        director->replaceScene(transition);
    }
}

void LevelAreaInnerLayer::onDoor(cocos2d::CCObject* sender) {
    if (m_enteringLevel) {
        return;
    }

    m_enteringLevel = true;
    m_levelID = sender->getTag();

    auto audioEngine = FMODAudioEngine::sharedEngine();
    audioEngine->playEffect("playSound_01.ogg", 1.0f, 0.0f, 0.3f);

    // Black fade
    auto fadeLayer = cocos2d::CCLayerColor::create();
    addChild(fadeLayer, 90);
    fadeLayer->runAction(cocos2d::CCFadeIn::create(0.5f));

    audioEngine->fadeOutMusic(0.5f, 0);

    auto delay = cocos2d::CCDelayTime::create(0.5f);
    auto playAction = cocos2d::CCCallFunc::create(this, callfunc_selector(LevelAreaInnerLayer::playStep1));
    runAction(cocos2d::CCSequence::create(delay, playAction, nullptr));
}

void LevelAreaInnerLayer::onExit() {
    cocos2d::CCLayer::onExit();

    if (m_exiting) {
        auto spriteFrameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
        spriteFrameCache->removeSpriteFramesFromFile("TowerSheet.plist");

        auto textureCache = cocos2d::CCTextureCache::sharedTextureCache();
        textureCache->removeTextureForKey("TowerSheet.png");
    }
}

void LevelAreaInnerLayer::onInfo(cocos2d::CCObject* sender) {
    auto urlArray = cocos2d::CCArray::create();

    // Music credits
    struct Credit {
        const char* name;
        const char* url;
    };

    Credit credits[] = {
        {"Kevin MacLeod", "https://incompetech.com/"},
        {"Sebaravila", "https://www.youtube.com/user/SebaRavila"},
        {"Cyberwave Orchestra", "https://www.youtube.com/user/smashMania"},
        {"Daniel Carl", "https://www.youtube.com/user/danielcarlmusic"},
        {"Boom Kitty", "https://www.youtube.com/user/boomkitty"},
        {"Skyth", "https://www.youtube.com/user/SkythMusic"}
    };

    for (auto& credit : credits) {
        auto urlObj = CCURLObject::create(credit.name, credit.url);
        urlArray->addObject(urlObj);
    }

    auto creditLayer = URLViewLayer::create("Music Credits", urlArray);
    addChild(creditLayer, 100);
    creditLayer->showLayer(false);
}

void LevelAreaInnerLayer::onNextFloor(cocos2d::CCObject* sender) {
    if (m_enteringLevel) {
        return;
    }

    std::string message = "The Tower Second Floor is...\n<cg>Coming Soon</c>";
    auto alert = FLAlertLayer::create(nullptr, "Locked", message.c_str(), "OK", nullptr, nullptr);
    alert->show();
}

void LevelAreaInnerLayer::onOnlineVault(cocos2d::CCObject* sender) {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = SecretLayer5::scene();
    auto transition = cocos2d::CCTransitionFade::create(1.0f, scene);
    director->replaceScene(transition);
}

bool LevelAreaInnerLayer::playStep1() {
    auto gameManager = GameManager::sharedState();
    gameManager->m_sceneEnum = 15;
    m_exiting = true;

    auto levelManager = GameLevelManager::sharedState();
    auto mainLevel = levelManager->getMainLevel(m_levelID, true);

    auto localLevelManager = LocalLevelManager::sharedState();
    std::string levelString = localLevelManager->getMainLevelString(m_levelID);
    mainLevel->m_levelString = levelString;

    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = PlayLayer::scene(mainLevel, false, 0);
    auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    director->replaceScene(transition);

    return true;
}

cocos2d::CCScene* LevelAreaInnerLayer::scene(bool returning) {
    auto scene = cocos2d::CCScene::create();
    auto layer = LevelAreaInnerLayer::create(returning);
    scene->addChild(layer);
    return scene;
}

void LevelAreaInnerLayer::showFloor1CompleteDialog() {
    auto gameManager = GameManager::sharedState();
    gameManager->setUGV("33", true);

    auto dialogArray = cocos2d::CCArray::create();

    struct DialogEntry {
        const char* speaker;
        const char* text;
        int iconID;
        float speed;
        int unknown;
        cocos2d::ccColor3B color;
    };

    DialogEntry dialogs[] = {
        {"Scratch", "Where have you been?!\nI've been looking everywhere for you!", 14, 1.0f, 0, cocos2d::ccc3(255, 255, 255)},
        {"Scratch", "<co>You really shouldn't be poking around here.</c>", 11, 1.0f, 0, cocos2d::ccc3(255, 255, 255)},
        {"Scratch", "Never know what you could find.\n<d050><co>Or who<d010>.<d010>.<d010>.</c>", 13, 1.0f, 0, cocos2d::ccc3(255, 255, 255)},
        {"Scratch", "<cl><d010>.<d010>.<d010>.<d050> I thought this was a simple cube</c> <cl>game.</c>", 26, 1.0f, 0, cocos2d::ccc3(255, 255, 255)},
        {"Scratch", ".<d010>.<d010>.", 8, 1.0f, 0, cocos2d::ccc3(255, 255, 255)}
    };

    for (auto& dialog : dialogs) {
        auto dialogObj = DialogObject::create(
            dialog.speaker,
            dialog.text,
            dialog.iconID,
            dialog.speed,
            dialog.unknown,
            dialog.color
        );
        dialogArray->addObject(dialogObj);
    }

    auto dialogLayer = DialogLayer::createWithObjects(dialogArray, 2);
    auto runningScene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
    runningScene->addChild(dialogLayer, 1000);

    dialogLayer->m_delegate = this;
    dialogLayer->updateChatPlacement(Center);
    dialogLayer->animateInRandomSide();
}

void LevelAreaInnerLayer::tryResumeTowerMusic() {
    auto gameManager = GameManager::sharedState();
    gameManager->fadeInMusic("tower01.mp3");
}

void LevelAreaInnerLayer::tryShowAd() {
    auto gameManager = GameManager::sharedState();
    gameManager->tryShowInterstitial(120, 140, 0);
}

LevelAreaInnerLayer::~LevelAreaInnerLayer() {
    // Destructor
}