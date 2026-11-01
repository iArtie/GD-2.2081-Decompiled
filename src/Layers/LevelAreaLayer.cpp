#include "Layers/LevelAreaLayer.h"
#include "Layers/LevelAreaInnerLayer.h"
#include "Layers/LevelSelectLayer.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Audio/FMODAudioEngine.h"
#include "Utils/PlatformToolbox.h"
#include "Models/DialogObject.h"
#include "Cocos/CCSpriteGrayscale.h"

LevelAreaLayer* LevelAreaLayer::create() {
    auto layer = new LevelAreaLayer();
    if (layer && layer->init()) {
        layer->autorelease();
        return layer;
    }
    delete layer;
    return nullptr;
}

bool LevelAreaLayer::init() {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }

    auto gameManager = GameManager::sharedState();
    gameManager->fadeInMusic("tower01.mp3");

    m_godRays = cocos2d::CCArray::create();
    m_godRays->retain();

    auto textureCache = cocos2d::CCTextureCache::sharedTextureCache();
    textureCache->addImage("TowerSheet.png");

    auto spriteFrameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
    spriteFrameCache->addSpriteFramesWithFile("TowerSheet.plist");

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    auto director = cocos2d::CCDirector::sharedDirector();
    auto winSize = director->getWinSize();

    auto backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    auto backButton = CCMenuItemSpriteExtra::create(
        backSprite,
        nullptr,
        this,
        menu_selector(LevelAreaLayer::onBack)
    );
    backButton->setSizeMult(1.6f);

    auto backMenu = cocos2d::CCMenu::create(backButton, nullptr);
    addChild(backMenu, 1);
    backMenu->setPosition(
        director->getScreenLeft() + 24.0f,
        director->getScreenTop() - 23.0f
    );

    if (PlatformToolbox::isControllerConnected()) {
        GameToolbox::addBackButton(this, backButton);
    }

    auto bgTexture = gameManager->getBGTexture(23);
    auto bgSprite = cocos2d::CCSprite::create(bgTexture);
    bgSprite->getTexture()->setTexParameters({
        GL_LINEAR, GL_LINEAR,
        GL_REPEAT, GL_REPEAT
    });
    addChild(bgSprite, -3);
    bgSprite->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
    bgSprite->setScale(winSize.height / bgSprite->getContentSize().height);
    bgSprite->setPosition(0.0f, -50.0f);
    bgSprite->setColor(cocos2d::ccc3(60, 60, 180));

    auto gradient = cocos2d::CCLayerGradient::create(
        cocos2d::ccc4(0, 0, 0, 0),
        cocos2d::ccc4(255, 255, 255, 150)
    );
    gradient->setAnchorPoint(cocos2d::CCPoint(0.0f, 1.0f));
    gradient->setOrderOfArrival(770);
    gradient->setScaleY(0.7f);
    addChild(gradient, 2);

    auto fadeIn = cocos2d::CCFadeTo::create(3.0f, 255);
    auto fadeOut = cocos2d::CCFadeTo::create(3.0f, 150);
    auto easeIn = cocos2d::CCEaseInOut::create(fadeIn, 2.0f);
    auto easeOut = cocos2d::CCEaseInOut::create(fadeOut, 2.0f);
    auto gradientSequence = cocos2d::CCSequence::create(easeIn, easeOut, nullptr);
    gradient->runAction(cocos2d::CCRepeatForever::create(gradientSequence));

    auto towerSprite = cocos2d::CCSprite::createWithSpriteFrameName("theTower_01_001.png");
    m_towerSprite = towerSprite;
    addChild(towerSprite, 1);
    towerSprite->setScale(1.0f);
    towerSprite->setPosition(winSize.width * 0.5f, winSize.height * 0.5f);

    auto doorSprite = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
    doorSprite->setScaleX(2.0f);
    doorSprite->setScaleY(2.2f);
    doorSprite->setOpacity(0);

    auto doorButton = CCMenuItemSpriteExtra::create(
        doorSprite,
        nullptr,
        this,
        menu_selector(LevelAreaLayer::onClickDoor)
    );
    doorButton->setActivateSound("door02.ogg");

    auto doorMenu = cocos2d::CCMenu::create(doorButton, nullptr);
    addChild(doorMenu, 10);

    auto towerPos = towerSprite->getPosition();
    auto doorPos = towerPos + cocos2d::CCPoint(0.0f, -75.0f);
    doorButton->setPosition(doorMenu->convertToNodeSpace(doorPos));

    // God rays - first ray
    auto godRay1 = cocos2d::CCSprite::createWithSpriteFrameName("godray01_001.png");
    godRay1->setColor(cocos2d::ccc3(0, 0, 50));
    addChild(godRay1, -1);
    godRay1->setScaleX(winSize.width / godRay1->getContentSize().width);
    godRay1->setScaleY(3.0f);
    godRay1->setRotation(90.0f);
    godRay1->setPosition(0.0f, winSize.height * 0.5f);
    godRay1->setOpacity(125);
    godRay1->setColor(cocos2d::ccc3(0, 0, 50));

    // God rays - second ray
    auto godRay2 = cocos2d::CCSprite::createWithSpriteFrameName("godray01_001.png");
    godRay2->setColor(cocos2d::ccc3(0, 0, 50));
    addChild(godRay2, -1);
    godRay2->setScaleX(winSize.width / godRay2->getContentSize().width);
    godRay2->setScaleY(3.0f);
    godRay2->setRotation(90.0f);
    godRay2->setPosition(winSize.width, winSize.height * 0.5f);
    godRay2->setOpacity(125);
    godRay2->setColor(cocos2d::ccc3(0, 0, 50));

    // Bottom god ray
    auto bottomGodRay = cocos2d::CCSprite::createWithSpriteFrameName("godray01_001.png");
    bottomGodRay->setColor(cocos2d::ccc3(0, 0, 50));
    addChild(bottomGodRay, -1);
    bottomGodRay->setScaleX(winSize.width / bottomGodRay->getContentSize().width);
    bottomGodRay->setScaleY(1.0f);
    bottomGodRay->setPosition(winSize.width * 0.5f, 0.0f);
    bottomGodRay->setOpacity(125);
    bottomGodRay->setColor(cocos2d::ccc3(0, 50, 0));

    // Floating animation for tower
    towerSprite->setPosition(towerSprite->getPosition() + cocos2d::CCPoint(0.0f, -2.0f));
    auto moveUp = cocos2d::CCMoveBy::create(2.0f, cocos2d::CCPoint(0.0f, 4.0f));
    auto moveDown = cocos2d::CCMoveBy::create(2.0f, cocos2d::CCPoint(0.0f, -4.0f));
    auto easeIn2 = cocos2d::CCEaseInOut::create(moveUp, 2.0f);
    auto easeOut2 = cocos2d::CCEaseInOut::create(moveDown, 2.0f);
    auto towerSequence = cocos2d::CCSequence::create(easeIn2, easeOut2, nullptr);
    towerSprite->runAction(cocos2d::CCRepeatForever::create(towerSequence));

    // More god rays at various positions
    auto centerPos = cocos2d::CCPoint(winSize.width * 0.5f, (winSize.height * 0.5f) + 13.0f);
    
    // God ray at center
    auto centerRay = cocos2d::CCSprite::createWithSpriteFrameName("godray01_001.png");
    centerRay->setColor(cocos2d::ccc3(50, 0, 125));
    addChild(centerRay, -1);
    centerRay->setScaleX(winSize.width / centerRay->getContentSize().width);
    centerRay->setScaleY(3.0f);
    centerRay->setRotation(90.0f);
    centerRay->setPosition(centerPos);
    centerRay->setOpacity(125);
    centerRay->setColor(cocos2d::ccc3(50, 0, 125));

    // Additional god rays
    auto rayPositions = {
        cocos2d::CCPoint(0.0f, 10.0f),
        cocos2d::CCPoint(0.0f, 20.0f),
        cocos2d::CCPoint(0.0f, -20.0f),
        cocos2d::CCPoint(0.0f, -10.0f),
        cocos2d::CCPoint(0.0f, 10.0f)
    };
    
    cocos2d::ccColor3B rayColors[] = {
        cocos2d::ccc3(125, 25, 125),
        cocos2d::ccc3(75, 0, 100),
        cocos2d::ccc3(0, 100, 25),
        cocos2d::ccc3(0, 75, 25)
    };
    
    float rayScales[] = {4.0f, 3.0f, 6.0f, 5.0f, 4.5f};
    float rayDurations[] = {5.0f, 4.0f, 6.0f, 5.0f, 4.5f};

    int rayIndex = 0;
    for (auto offset : rayPositions) {
        if (rayIndex >= 4) break;
        auto pos = centerPos + offset;
        auto ray = cocos2d::CCSprite::createWithSpriteFrameName("godray01_001.png");
        ray->setColor(rayColors[rayIndex]);
        addChild(ray, -1);
        ray->setScaleX(winSize.width / ray->getContentSize().width);
        ray->setScaleY(rayScales[rayIndex]);
        ray->setRotation(90.0f);
        ray->setPosition(pos);
        ray->setOpacity(125);
        ray->setColor(rayColors[rayIndex]);
        rayIndex++;
    }

    // Particle system
    std::string particleString = "60a-1a8a2a6a90a90a0a2a45a70a0a0a0a0a0a0a1a0a0a0a1a0a0.886275a0a0a0a0a0.7a1a0a0a0a1a0a0.552941a0a0a0a0a0.7a2a0a2a0a0a0a0a0a0a0a0a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0";
    auto particle = GameToolbox::particleFromString(particleString, nullptr, nullptr);
    addChild(particle, 0);
    particle->setRotation(30.0f);
    particle->setPosVar(cocos2d::CCPoint(100.0f, 40.0f));
    particle->setPosition(centerPos + cocos2d::CCPoint(-100.0f, 60.0f));

    // Update particle 20 times
    for (int i = 0; i < 20; ++i) {
        particle->update(0.2f);
    }

    // Torch
    auto torchPos = cocos2d::CCPoint(
        towerSprite->getContentSize().width * 0.5f + 35.0f,
        towerSprite->getContentSize().height * 0.5f - 69.5f
    );
    LevelAreaLayer::addTorch(towerSprite, 0.4f, torchPos, 0.0f, 1, 6, 0, 0);

    // Show dialog if first time
    if (!gameManager->getUGV("29")) {
        m_enteringTower = true;
        auto delay = cocos2d::CCDelayTime::create(1.2f);
        auto showDialogAction = cocos2d::CCCallFunc::create(this, callfunc_selector(LevelAreaLayer::showDialog));
        runAction(cocos2d::CCSequence::create(delay, showDialogAction, nullptr));
    }

    return true;
}

void LevelAreaLayer::keyBackClicked() {
    onBack(nullptr);
}

void LevelAreaLayer::onBack(cocos2d::CCObject* sender) {
    if (!m_enteringTower) {
        setKeypadEnabled(false);
        auto gameManager = GameManager::sharedState();
        gameManager->fadeInMenuMusic();
        m_exiting = true;
        auto director = cocos2d::CCDirector::sharedDirector();
        auto scene = LevelSelectLayer::scene(2);
        auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
        director->replaceScene(transition);
    }
}

void LevelAreaLayer::onClickDoor(cocos2d::CCObject* sender) {
    if (m_enteringTower) {
        return;
    }

    m_enteringTower = true;
    setKeypadEnabled(false);

    if (m_towerSprite) {
        // Scale tower up and move it
        auto scaleTo = cocos2d::CCScaleTo::create(1.4f, 20.0f);
        auto easeScale = cocos2d::CCEaseExponentialIn::create(scaleTo);
        m_towerSprite->runAction(easeScale);

        auto moveBy = cocos2d::CCMoveBy::create(1.4f, cocos2d::CCPoint(0.0f, 1550.0f));
        auto easeMove = cocos2d::CCEaseExponentialIn::create(moveBy);
        m_towerSprite->runAction(easeMove);

        // Fade out god rays
        for (int i = 0; i < m_godRays->count(); ++i) {
            auto node = static_cast<cocos2d::CCNode*>(m_godRays->objectAtIndex(i));
            node->stopAllActions();
            node->runAction(cocos2d::CCFadeTo::create(1.0f, 0));
        }

        // Fade inside tower
        auto delay = cocos2d::CCDelayTime::create(1.1f);
        auto fadeAction = cocos2d::CCCallFunc::create(this, callfunc_selector(LevelAreaLayer::fadeInsideTower));
        runAction(cocos2d::CCSequence::create(delay, fadeAction, nullptr));
    }
}

void LevelAreaLayer::fadeInsideTower() {
    auto fadeLayer = cocos2d::CCLayerColor::create();
    addChild(fadeLayer, 90);
    fadeLayer->runAction(cocos2d::CCFadeIn::create(0.3f));

    auto delay = cocos2d::CCDelayTime::create(0.3f);
    auto enterAction = cocos2d::CCCallFunc::create(this, callfunc_selector(LevelAreaLayer::onEnterTower));
    runAction(cocos2d::CCSequence::create(delay, enterAction, nullptr));
}

bool LevelAreaLayer::onEnterTower() {
    auto audioEngine = FMODAudioEngine::sharedEngine();
    audioEngine->playEffect("door01.ogg");

    auto director = cocos2d::CCDirector::sharedDirector();
    auto scene = LevelAreaInnerLayer::scene(false);
    director->replaceScene(scene);

    return true;
}

void LevelAreaLayer::onExit() {
    cocos2d::CCLayer::onExit();

    if (m_exiting) {
        auto spriteFrameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
        spriteFrameCache->removeSpriteFramesFromFile("TowerSheet.plist");

        auto textureCache = cocos2d::CCTextureCache::sharedTextureCache();
        textureCache->removeTextureForKey("TowerSheet.png");
    }
}

cocos2d::CCScene* LevelAreaLayer::scene() {
    auto scene = cocos2d::CCScene::create();
    auto layer = LevelAreaLayer::create();
    scene->addChild(layer);
    return scene;
}

void LevelAreaLayer::showDialog() {
    auto gameManager = GameManager::sharedState();
    gameManager->setUGV("29", true);

    auto dialogArray = cocos2d::CCArray::create();

    // Dialog 1
    auto dialog1 = DialogObject::create(
        "Scratch",
        "Who left the <cy>lights</c> on?",
        13,
        1.0f,
        0,
        cocos2d::ccc3(255, 255, 255)
    );
    dialogArray->addObject(dialog1);

    // Dialog 2
    auto dialog2 = DialogObject::create(
        "Scratch",
        "<co>I got a bad feeling about this<d010>.<d010>.<d010>.</c>",
        11,
        1.0f,
        0,
        cocos2d::ccc3(255, 255, 255)
    );
    dialogArray->addObject(dialog2);

    auto dialogLayer = DialogLayer::createWithObjects(dialogArray, 2);
    dialogLayer->m_delegate = this;

    auto runningScene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
    runningScene->addChild(dialogLayer, 1000);

    dialogLayer->m_delegate = this;
    dialogLayer->updateChatPlacement(Center);
    dialogLayer->animateInRandomSide();
}

void LevelAreaLayer::dialogClosed(DialogLayer* layer) {
    m_enteringTower = false;
}

LevelAreaLayer::~LevelAreaLayer() {
    if (m_godRays) {
        m_godRays->release();
    }
}