#include "Pages/LevelPage.h"
#include "Models/GJGameLevel.h"
#include "Objects/GameObject.h"
#include "Managers/GameManager.h"
#include "Managers/GameStatsManager.h"
#include "Managers/LocalLevelManager.h"
#include "Utils/GameToolbox.h"
#include "Delegates/DialogDelegate.h"
#include "Layers/DialogLayer.h"
#include "Models/DialogObject.h"
#include "Layers/PlayLayer.h"
#include "Layers/SecretLayer4.h"
#include "Layers/LevelAreaLayer.h"
#include "Layers/GJMoreGamesLayer.h"
#include "Cocos/CCCircleWave.h"
#include "Cocos/CCMenuItemSpriteExtra.h"
#include "Utils/PlatformToolbox.h"
#include "Audio/FMODAudioEngine.h"
#include "cocos-ext.h"

static bool s_didClick = false;

void LevelPage::addSecretCoin() {
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    GameObject* coin = GameObject::createWithFrame("secretCoin_01_001.png");
    m_coinObject = coin;
    coin->m_bUnkBool2 = false;
    this->addChild(coin, 1);
    coin->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f));

    cocos2d::CCArray* frames = cocos2d::CCArray::create();
    for (int i = 1; i <= 4; ++i) {
        cocos2d::CCString* key = cocos2d::CCString::createWithFormat("secretCoin_01_%03d.png", i);
        cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(key->getCString());
        frames->addObject(frame);
    }

    cocos2d::CCAnimation* anim = cocos2d::CCAnimation::createWithSpriteFrames(frames, 0.115f);
    cocos2d::CCAnimate* animate = cocos2d::CCAnimate::create(anim);
    cocos2d::CCRepeatForever* repeat = cocos2d::CCRepeatForever::create(animate);
    m_coinObject->runAction(repeat);
}

void LevelPage::addSecretDoor() {
    GameManager* GM = GameManager::sharedState();
    if (!GM->getUGV("6")) {
        return;
    }

    bool opened = GM->getUGV("8");
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    const char* frame = opened ? "secretDoorBtn2_open_001.png" : "secretDoorBtn2_closed_001.png";
    m_secretDoor = cocos2d::CCSprite::createWithSpriteFrameName(frame);

    if (!GM->getUGV("7")) {
        m_secretDoor->setOpacity(0);
    }

    cocos2d::CCMenu* menu = cocos2d::CCMenu::create();
    this->addChild(menu);

    CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(
        m_secretDoor, nullptr, this, menu_selector(LevelPage::onSecretDoor));
    item->m_animationEnabled = false;
    item->m_colorEnabled = true;
    menu->addChild(item);

    float menuY = winSize.height * 0.5f - 110.0f;
    if (menuY > 128.0f) {
        menuY = 128.0f;
    } else {
        menuY += 30.0f;
    }
    menu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, menuY));
    item->setSizeMult(2.0f);

    m_dynamicObjects->addObject(item);
    m_dynamicObjects->addObject(m_secretDoor);
}

bool LevelPage::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (!m_level || !m_coinObject) {
        return false;
    }
    if (m_level->m_levelID.random - m_level->m_levelID.seed != -1) {
        return false;
    }

    cocos2d::CCPoint touchPos = touch->getLocation();
    cocos2d::CCPoint localPos = this->convertToNodeSpace(touchPos);

    if (cocos2d::ccpDistance(m_coinObject->getPosition(), localPos) <= 30.0f) {
        setTouchEnabled(false);
        playCoinEffect();

        GameManager* GM = GameManager::sharedState();
        GM->reportAchievementWithID("geometry.ach.secret04", 100, false);

        GameStatsManager* GSM = GameStatsManager::sharedState();
        GSM->storeSecretCoin("secret04");
        GSM->incrementStat("8");
    }
    return true;
}

void LevelPage::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    ccTouchEnded(touch, event);
}

void LevelPage::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
}

void LevelPage::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
}

LevelPage* LevelPage::create(GJGameLevel* level) {
    LevelPage* page = new LevelPage();
    if (page && page->init(level)) {
        page->autorelease();
        return page;
    }
    CC_SAFE_DELETE(page);
    return nullptr;
}

void LevelPage::dialogClosed(DialogLayer* layer) {
    if (layer->getTag() == 2) {
        GameManager::sharedState()->setUGV("8", true);
        if (m_secretDoor) {
            m_secretDoor->setDisplayFrame(
                cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("secretDoorBtn2_open_001.png"));
        }
    } else if (layer->getTag() == 1) {
        onTheTower(nullptr);
    }
}

bool LevelPage::init(GJGameLevel* level) {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }

    setTouchEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    m_levelDisplaySize = cocos2d::CCSize(340.0f, 95.0f);

    m_progressObjects = cocos2d::CCArray::create();
    m_progressObjects->retain();

    m_levelObjects = cocos2d::CCArray::create();
    m_levelObjects->retain();

    m_levelMenu = cocos2d::CCMenu::create();
    this->addChild(m_levelMenu, -1);
    m_levelMenu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 60.0f));

    m_levelDisplay = cocos2d::extension::CCScale9Sprite::create(
        "square02_001.png", cocos2d::CCRect(0.0f, 0.0f, 80.0f, 80.0f));
    m_levelDisplay->setOpacity(125);
    m_levelDisplay->setContentSize(m_levelDisplaySize);
    m_levelDisplay->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 50.0f));

    cocos2d::CCSprite* displaySprite = cocos2d::CCSprite::create();
    displaySprite->addChild(m_levelDisplay);
    displaySprite->setContentSize(m_levelDisplay->getContentSize());
    m_levelDisplay->setPosition(cocos2d::CCPoint(
        m_levelDisplay->getContentSize().width * 0.5f,
        m_levelDisplay->getContentSize().height * 0.5f));

    CCMenuItemSpriteExtra* playBtn = CCMenuItemSpriteExtra::create(
        displaySprite, nullptr, this, menu_selector(LevelPage::onPlay));
    playBtn->m_scaleMultiplier = 1.1f;
    m_levelMenu->addChild(playBtn);
    m_progressObjects->addObject(m_levelMenu);

    cocos2d::CCSprite* normalBg = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    normalBg->setColor(cocos2d::ccc3(0, 0, 0));
    normalBg->setOpacity(125);
    normalBg->setScale(1.0f);
    this->addChild(normalBg, 3);
    normalBg->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 30.0f));
    m_progressObjects->addObject(normalBg);

    m_normalProgressBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    m_normalProgressBar->setScaleX(0.992f);
    m_normalProgressBar->setScaleY(0.86f);
    m_normalProgressBar->setColor(cocos2d::ccc3(50, 255, 50));
    m_progressWidth = m_normalProgressBar->getContentSize().width;
    normalBg->addChild(m_normalProgressBar, 1);
    m_normalProgressBar->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    float offset = m_progressWidth - m_progressWidth * 0.992f;
    m_normalProgressBar->setPosition(cocos2d::CCPoint(
        offset * 0.5f,
        normalBg->getContentSize().height * 0.5f));

    cocos2d::CCSprite* practiceBg = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    practiceBg->setColor(cocos2d::ccc3(0, 0, 0));
    practiceBg->setOpacity(125);
    practiceBg->setScale(1.0f);
    this->addChild(practiceBg, 3);
    practiceBg->setPosition(normalBg->getPosition() + cocos2d::CCPoint(0.0f, -50.0f));
    m_progressObjects->addObject(practiceBg);

    m_practiceProgressBar = cocos2d::CCSprite::create("GJ_progressBar_001.png");
    m_practiceProgressBar->setScaleX(0.992f);
    m_practiceProgressBar->setScaleY(0.86f);
    m_practiceProgressBar->setColor(cocos2d::ccc3(50, 255, 255));
    practiceBg->addChild(m_practiceProgressBar, 1);
    m_practiceProgressBar->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_practiceProgressBar->setPosition(m_normalProgressBar->getPosition());

    m_normalProgressLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    this->addChild(m_normalProgressLabel, 4);
    m_normalProgressLabel->setPosition(normalBg->getPosition());
    m_normalProgressLabel->setScale(0.5f);
    m_progressObjects->addObject(m_normalProgressLabel);

    m_practiceProgressLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    this->addChild(m_practiceProgressLabel, 4);
    m_practiceProgressLabel->setPosition(practiceBg->getPosition());
    m_practiceProgressLabel->setScale(0.5f);
    m_progressObjects->addObject(m_practiceProgressLabel);

    cocos2d::CCLabelBMFont* normalTitle = cocos2d::CCLabelBMFont::create("Normal", "bigFont.fnt");
    this->addChild(normalTitle, 4);
    normalTitle->setPosition(normalBg->getPosition() + cocos2d::CCPoint(0.0f, 20.0f));
    normalTitle->setScale(0.5f);
    m_progressObjects->addObject(normalTitle);

    cocos2d::CCLabelBMFont* practiceTitle = cocos2d::CCLabelBMFont::create("Practice", "bigFont.fnt");
    this->addChild(practiceTitle, 4);
    practiceTitle->setPosition(practiceBg->getPosition() + cocos2d::CCPoint(0.0f, 20.0f));
    practiceTitle->setScale(0.5f);
    m_progressObjects->addObject(practiceTitle);

    m_nameLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_nameLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
    m_levelDisplay->addChild(m_nameLabel);
    m_levelObjects->addObject(m_nameLabel);

    m_difficultySprite = cocos2d::CCSprite::createWithSpriteFrameName("diffIcon_01_btn_001.png");
    m_levelDisplay->addChild(m_difficultySprite);
    m_difficultySprite->setScale(1.1f);
    m_levelObjects->addObject(m_difficultySprite);

    m_starsSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
    m_levelDisplay->addChild(m_starsSprite);
    m_starsSprite->setScale(0.7f);
    m_starsSprite->setPosition(cocos2d::CCPoint(
        m_levelDisplaySize.width - 15.0f,
        m_levelDisplaySize.height - 13.0f));
    m_levelObjects->addObject(m_starsSprite);

    m_starsLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    m_levelDisplay->addChild(m_starsLabel);
    m_starsLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
    m_starsLabel->setPosition(m_starsSprite->getPosition() + cocos2d::CCPoint(-12.0f, 0.5f));
    m_starsLabel->setScale(0.5f);
    m_levelObjects->addObject(m_starsLabel);

    m_coins = cocos2d::CCArray::create();
    m_coins->retain();

    cocos2d::CCPoint coinStart(m_levelDisplaySize.width - 16.0f, 16.0f);
    for (int i = 2; i >= 0; --i) {
        cocos2d::CCSprite* coin = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon_001.png");
        m_levelDisplay->addChild(coin);
        coin->setScale(1.0f);
        coin->setPosition(coinStart + cocos2d::CCPoint(i * -26.0f, 0.0f));
        m_coins->addObject(coin);
        m_levelObjects->addObject(coin);
    }

    m_dynamicObjects = cocos2d::CCArray::create();
    m_dynamicObjects->retain();

    if (PlatformToolbox::isControllerConnected()) {
        cocos2d::CCSprite* controllerBtn = cocos2d::CCSprite::createWithSpriteFrameName("controllerBtn_Start_001.png");
        displaySprite->addChild(controllerBtn, 10);
        controllerBtn->setPosition(m_levelDisplay->getPosition() + cocos2d::CCPoint(0.0f, -35.0f));
        m_levelObjects->addObject(controllerBtn);
    }

    return true;
}

void LevelPage::onInfo(cocos2d::CCObject* sender) {
    if (!m_level) {
        return;
    }

    int levelID = m_level->m_levelID.random - m_level->m_levelID.seed;

    if (levelID == -1) {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "It's a secret...",
            "<cr>Roses are red</c>\n<cl>Violets are blue</c>\n<cg>Welcome to</c>\n<cy>2.2</c>");
        alert->show();
    } else if (levelID == -2) {
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "The Tower",
            "The path leads to an <cr>old tower</c>. It's been left alone for <cg>years</c>, with little reason to <co>explore</c>.");
        alert->show();
    } else {
        int attempts = m_level->m_attempts.random - m_level->m_attempts.seed;
        int jumps = m_level->m_jumps.random - m_level->m_jumps.seed;
        int normalPercent = m_level->getNormalPercent();

        std::string stats = cocos2d::CCString::createWithFormat(
            "<cy>%s</c>\n"
            "<cg>Total Attempts</c>: %i\n"
            "<cl>Total Jumps</c>: %i\n"
            "<cp>Normal</c>: %i%%\n"
            "<co>Practice</c>: %i%%",
            m_level->m_levelName.c_str(),
            attempts,
            jumps,
            normalPercent,
            m_level->m_practicePercent)->getCString();

        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Level Stats", stats.c_str());
        alert->show();
    }
}

void LevelPage::onMoreGames(cocos2d::CCObject* sender) {
    GameManager::sharedState()->reportAchievementWithID("geometry.ach.moreGames", 100, false);
    GJMoreGamesLayer* layer = GJMoreGamesLayer::create();
    cocos2d::CCDirector::sharedDirector()->m_pRunningScene->addChild(layer, 100);
    layer->showLayer(false);
}

void LevelPage::onPlay(cocos2d::CCObject* sender) {
    if (m_level->m_levelID.random - m_level->m_levelID.seed == -1) {
        return;
    }

    GameStatsManager* GSM = GameStatsManager::sharedState();
    int secretCoins = GSM->getStat("8");

    if (m_level->m_levelID.random - m_level->m_levelID.seed == -2) {
        onTheTower(nullptr);
        return;
    }

    int requiredCoins = m_level->m_requiredCoins;
    if (secretCoins < requiredCoins) {
        std::string msg = cocos2d::CCString::createWithFormat(
            "Collect %i more <cy>Secret Coins</c> to unlock this <cl>level</c>!",
            requiredCoins - secretCoins)->getCString();
        FLAlertLayer* alert = FLAlertLayer::create(nullptr, "Locked", msg.c_str());
        alert->show();
        return;
    }

    if (m_isBusy) {
        return;
    }

    m_isBusy = true;
    s_didClick = true;

    FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    FMODAudioEngine::sharedEngine()->playEffect("playSound_01.ogg", 1.0f, 0.0f, 0.3f);

    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(LevelPage::playStep2)),
        nullptr
    );
    this->runAction(seq);
}

void LevelPage::onSecretDoor(cocos2d::CCObject* sender) {
    if (s_didClick) {
        return;
    }

    GameManager* GM = GameManager::sharedState();

    if (!GM->getUGV("7")) {
        if (m_secretDoor) {
            GM->setUGV("7", true);
            cocos2d::CCSprite* door = cocos2d::CCSprite::createWithSpriteFrameName("secretDoorBtn2_closed_001.png");
            m_secretDoor->addChild(door, 2);
            door->setPosition(cocos2d::CCPoint(
                m_secretDoor->getContentSize().width * 0.5f,
                m_secretDoor->getContentSize().height * 0.5f));
            door->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
            m_secretDoor->setOpacity(255);
            m_secretDoor->setScale(2.0f);
            m_secretDoor->runAction(cocos2d::CCEaseBounceOut::create(cocos2d::CCScaleTo::create(0.5f, 1.0f)));
            door->runAction(cocos2d::CCFadeOut::create(0.5f));
        }
        return;
    }

    if (GM->getUGV("8")) {
        s_didClick = true;
        cocos2d::CCScene* scene = SecretLayer4::scene();
        cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(fade);
        return;
    }

    GameStatsManager* GSM = GameStatsManager::sharedState();
    bool hasEmblem = GSM->isItemUnlocked(UnlockType::GJItem, 4);
    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    if (hasEmblem) {
        DialogObject* d1 = DialogObject::create("The Gatekeeper", "The <cp>Master Emblem</c>!", 7, 1.0f, true, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d1);
        DialogObject* d2 = DialogObject::create("The Gatekeeper", "Lord <cl>RubRub</c>, I will open the gate for you.", 7, 0.8f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d2);
        DialogObject* d3 = DialogObject::create("The Gatekeeper", "Please, come in.", 7, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d3);

        DialogLayer* layer = DialogLayer::createWithObjects(dialogs, 2);
        layer->setDelegate(this);
        layer->setTag(2);
        cocos2d::CCDirector::sharedDirector()->m_pRunningScene->addChild(layer, 1000);
        layer->updateChatPlacement(DialogChatPlacement::Center);
        layer->animateIn(DialogAnimationType::FromLeft);
    } else {
        DialogObject* d1 = DialogObject::create("The Gatekeeper", "Who disturbs my slumber?", 7, 1.0f, true, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d1);
        DialogObject* d2 = DialogObject::create("The Gatekeeper", "<cl>RubRub</c> is that you?<d040>\nI cannot see in this light.", 7, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d2);
        DialogObject* d3 = DialogObject::create("The Gatekeeper", "Show me the <cp>Master Emblem</c>,\n<d030>and I will let you pass.", 7, 1.0f, false, cocos2d::ccc3(255, 255, 255));
        dialogs->addObject(d3);

        DialogLayer* layer = DialogLayer::createWithObjects(dialogs, 2);
        cocos2d::CCDirector::sharedDirector()->m_pRunningScene->addChild(layer, 1000);
        layer->updateChatPlacement(DialogChatPlacement::Center);
        layer->animateIn(DialogAnimationType::FromLeft);
    }
}

void LevelPage::onTheTower(cocos2d::CCObject* sender) {
    GameManager* GM = GameManager::sharedState();

    if (GM->getUGV("28")) {
        cocos2d::CCScene* scene = LevelAreaLayer::scene();
        cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, scene);
        cocos2d::CCDirector::sharedDirector()->replaceScene(fade);
        GM->m_sceneEnum = 9;
        return;
    }

    cocos2d::CCArray* dialogs = cocos2d::CCArray::create();

    DialogObject* d1 = DialogObject::create("Scratch", "Where do you <cl>think</c> you're going?", 10, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    dialogs->addObject(d1);
    DialogObject* d2 = DialogObject::create("Scratch", "There's nothing over there, that place has been <cr>abandoned</c> for years.", 13, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    dialogs->addObject(d2);
    DialogObject* d3 = DialogObject::create("Scratch", "<d010>.<d010>.<d010>.", 8, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    dialogs->addObject(d3);
    DialogObject* d4 = DialogObject::create("Scratch", "We can take a <cg>quick</c> look if you want, <d030>but we can't stay long.", 13, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    dialogs->addObject(d4);
    DialogObject* d5 = DialogObject::create("Scratch", "<co>RubRub is always watching.<d010>.<d010>.<d010>.</c>", 12, 1.0f, false, cocos2d::ccc3(255, 255, 255));
    dialogs->addObject(d5);

    GM->setUGV("28", true);

    DialogLayer* layer = DialogLayer::createWithObjects(dialogs, 2);
    layer->setDelegate(this);
    cocos2d::CCDirector::sharedDirector()->m_pRunningScene->addChild(layer, 1000);
    layer->setTag(1);
    layer->m_noRemove = true;
    layer->updateChatPlacement(DialogChatPlacement::Center);
    layer->animateInRandomSide();
}

void LevelPage::playCoinEffect() {
    m_coinObject->setVisible(false);

    cocos2d::CCParticleSystemQuad* particle = cocos2d::CCParticleSystemQuad::create("coinPickupEffect.plist");
    this->addChild(particle, 1);
    particle->setPosition(m_coinObject->getPosition());

    cocos2d::CCCircleWave* wave1 = cocos2d::CCCircleWave::create(0, 25.0f, 5.0f, 0.3f, false);
    wave1->setPosition(m_coinObject->getPosition());
    this->addChild(wave1, 0);

    cocos2d::CCCircleWave* wave2 = cocos2d::CCCircleWave::create(5.0f, 60.0f, 0.3f, false);
    wave2->m_color = cocos2d::ccc3(255, 255, 0);
    wave2->setPosition(m_coinObject->getPosition());
    this->addChild(wave2, 0);
    wave2->m_filled = true;

    cocos2d::CCSprite* sprite = cocos2d::CCSprite::createWithSpriteFrameName("secretCoin_01_001.png");
    this->addChild(sprite, 2);
    sprite->setPosition(m_coinObject->getPosition());

    cocos2d::CCArray* frames = cocos2d::CCArray::create();
    for (int i = 1; i <= 4; ++i) {
        cocos2d::CCString* key = cocos2d::CCString::createWithFormat("secretCoin_01_%03d.png", i);
        cocos2d::CCSpriteFrame* frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(key->getCString());
        frames->addObject(frame);
    }
    cocos2d::CCAnimation* anim = cocos2d::CCAnimation::createWithSpriteFrames(frames, 0.05f);
    cocos2d::CCAnimate* animate = cocos2d::CCAnimate::create(anim);
    cocos2d::CCRepeatForever* repeat = cocos2d::CCRepeatForever::create(animate);
    sprite->runAction(repeat);

    cocos2d::CCPoint startPos = m_coinObject->getPosition();
    float randomX = startPos.x + ((static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * 50.0f);
    cocos2d::CCPoint p1(startPos.x, startPos.y + 90.0f);
    cocos2d::CCPoint p2(randomX, startPos.y + 180.0f);
    cocos2d::CCPoint p3(randomX, startPos.y - 80.0f);

    cocos2d::ccBezierConfig bezier;
    bezier.controlPoint_1 = p1;
    bezier.controlPoint_2 = p2;
    bezier.endPosition = p3;

    cocos2d::CCBezierTo* bezierAction = cocos2d::CCBezierTo::create(0.75f, bezier);
    cocos2d::CCCallFuncN* callFunc = cocos2d::CCCallFuncN::create(sprite, callfuncN_selector(LevelPage::removeFromParentAndCleanup));
    cocos2d::CCSequence* seq1 = cocos2d::CCSequence::create(bezierAction, callFunc, nullptr);
    sprite->runAction(seq1);

    cocos2d::CCSequence* seq2 = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.5f),
        cocos2d::CCFadeOut::create(0.3f),
        nullptr
    );
    sprite->runAction(seq2);
}

void LevelPage::playStep2() {
    FMODAudioEngine::sharedEngine()->loadMusic(m_level->getAudioFileName(), 1.0f, 0.0f, 1.0f, true, 0, 0, 0);

    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0f),
        cocos2d::CCCallFunc::create(this, callfunc_selector(LevelPage::playStep3)),
        nullptr
    );
    this->runAction(seq);
}

void LevelPage::playStep3() {
    GameManager::sharedState()->m_sceneEnum = 9;

    LocalLevelManager* LLM = LocalLevelManager::sharedState();
    m_level->m_levelString = LLM->getMainLevelString(m_level->m_levelID.random - m_level->m_levelID.seed);

    cocos2d::CCScene* scene = PlayLayer::scene(m_level, false, false);
    cocos2d::CCTransitionFade* fade = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(fade);
}

void LevelPage::registerWithTouchDispatcher() {
    cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void LevelPage::updateDynamicPage(GJGameLevel* level) {
    if (m_isBusy) {
        return;
    }

    if (m_level) {
        if (level->m_levelID.random - level->m_levelID.seed == m_level->m_levelID.random - m_level->m_levelID.seed) {
            return;
        }
        m_level->release();
        m_level = nullptr;
    }

    for (unsigned int i = 0; i < m_dynamicObjects->count(); ++i) {
        cocos2d::CCNode* obj = static_cast<cocos2d::CCNode*>(m_dynamicObjects->objectAtIndex(i));
        obj->removeFromParentAndCleanup(true);
    }
    m_dynamicObjects->removeAllObjects();

    m_secretDoor = nullptr;
    m_level = level;
    level->retain();

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    int levelID = m_level->m_levelID.random - m_level->m_levelID.seed;

    if (levelID > 0) {
        for (unsigned int i = 0; i < m_progressObjects->count(); ++i) {
            static_cast<cocos2d::CCNode*>(m_progressObjects->objectAtIndex(i))->setVisible(true);
        }
        for (unsigned int i = 0; i < m_levelObjects->count(); ++i) {
            static_cast<cocos2d::CCNode*>(m_levelObjects->objectAtIndex(i))->setVisible(true);
        }

        float normalFill = m_progressWidth * (static_cast<float>(m_level->getNormalPercent()) / 100.0f);
        if (normalFill > m_progressWidth) normalFill = m_progressWidth;
        m_normalProgressBar->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, normalFill, m_normalProgressBar->getContentSize().height));

        float practiceFill = m_progressWidth * (static_cast<float>(m_level->m_practicePercent) / 100.0f);
        if (practiceFill > m_progressWidth) practiceFill = m_progressWidth;
        m_practiceProgressBar->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, practiceFill, m_practiceProgressBar->getContentSize().height));

        m_normalProgressLabel->setString(cocos2d::CCString::createWithFormat("%i%%", m_level->getNormalPercent())->getCString());
        m_practiceProgressLabel->setString(cocos2d::CCString::createWithFormat("%i%%", m_level->m_practicePercent)->getCString());

        int secretCoins = GameStatsManager::sharedState()->getStat("8");
        int requiredCoins = m_level->m_requiredCoins;

        m_nameLabel->setScale(1.0f);
        m_nameLabel->setString(m_level->m_levelName.c_str());

        float width = m_nameLabel->getContentSize().width;
        if (width > 250.0f) {
            m_nameLabel->setScale(250.0f / width);
        }

        int diffFrame = m_level->m_difficulty;
        if (diffFrame <= 0) diffFrame = 1;
        if (diffFrame > 6) diffFrame = 6;
        std::string diffKey = cocos2d::CCString::createWithFormat("diffIcon_%02d_btn_001.png", diffFrame)->getCString();
        m_difficultySprite->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(diffKey.c_str()));

        m_difficultySprite->setPosition(cocos2d::CCPoint(
            m_levelDisplaySize.width * 0.5f - (m_nameLabel->getContentSize().width * m_nameLabel->getScale() + 30.0f) * 0.5f,
            m_levelDisplaySize.height * 0.5f + 3.0f));
        m_nameLabel->setPosition(cocos2d::CCPoint(
            m_difficultySprite->getPosition().x + 30.0f,
            m_levelDisplaySize.height * 0.5f + 3.0f));

        m_starsLabel->setString(cocos2d::CCString::createWithFormat("%i", m_level->m_stars.random - m_level->m_stars.seed)->getCString());
        if (GameStatsManager::sharedState()->hasCompletedStarLevel(m_level)) {
            m_starsLabel->setColor(cocos2d::ccc3(255, 255, 50));
        } else {
            m_starsLabel->setColor(cocos2d::ccc3(255, 255, 255));
        }

        for (int k = 0; k < 3; ++k) {
            cocos2d::CCSprite* coin = static_cast<cocos2d::CCSprite*>(m_coins->objectAtIndex(k));
            const char* coinKey = m_level->getCoinKey(k + 1);
            if (GameStatsManager::sharedState()->hasSecretCoin(coinKey)) {
                coin->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_coinsIcon_001.png"));
                coin->setColor(cocos2d::ccc3(255, 255, 255));
            } else {
                coin->setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_coinsIcon_gray_001.png"));
                coin->setColor(cocos2d::ccc3(165, 165, 165));
            }
        }

        if (secretCoins < requiredCoins) {
            for (unsigned int i = 0; i < m_levelObjects->count(); ++i) {
                static_cast<cocos2d::CCNode*>(m_levelObjects->objectAtIndex(i))->setVisible(false);
            }

            cocos2d::CCSprite* lock = cocos2d::CCSprite::createWithSpriteFrameName("GJLargeLock_001.png");
            m_levelDisplay->addChild(lock);
            lock->setPosition(cocos2d::CCPoint(
                m_levelDisplaySize.width * 0.5f,
                m_levelDisplaySize.height * 0.5f));

            cocos2d::CCSprite* coinIcon = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon_001.png");
            m_levelDisplay->addChild(coinIcon);
            coinIcon->setScale(1.0f);
            coinIcon->setPosition(cocos2d::CCPoint(m_levelDisplaySize.width - 15.0f, 15.0f));

            cocos2d::CCLabelBMFont* coinLabel = cocos2d::CCLabelBMFont::create(
                cocos2d::CCString::createWithFormat("%i/%i", secretCoins, requiredCoins)->getCString(), "bigFont.fnt");
            m_levelDisplay->addChild(coinLabel);
            coinLabel->setAnchorPoint(cocos2d::CCPoint(1.0f, 0.5f));
            coinLabel->setPosition(coinIcon->getPosition() + cocos2d::CCPoint(-15.0f, 0.5f));
            coinLabel->setScale(0.7f);

            m_dynamicObjects->addObject(lock);
            m_dynamicObjects->addObject(coinIcon);
            m_dynamicObjects->addObject(coinLabel);
        } else {
            int baseCurrency = GameStatsManager::sharedState()->getBaseCurrencyForLevel(m_level);
            int awardedCurrency = GameStatsManager::sharedState()->getAwardedCurrencyForLevel(m_level);

            cocos2d::CCSprite* orbIcon = cocos2d::CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
            m_levelDisplay->addChild(orbIcon);
            orbIcon->setScale(0.7f);

            cocos2d::CCLabelBMFont* orbLabel = cocos2d::CCLabelBMFont::create(
                cocos2d::CCString::createWithFormat("%i/%i", awardedCurrency, baseCurrency + static_cast<int>(baseCurrency * 0.25f))->getCString(), "bigFont.fnt");
            m_levelDisplay->addChild(orbLabel);
            orbLabel->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.5f));
            orbLabel->setPosition(cocos2d::CCPoint(8.0f, 12.0f));
            orbLabel->setScale(0.4f);

            orbIcon->setPosition(cocos2d::CCPoint(
                orbLabel->getPosition().x + orbLabel->getContentSize().width * orbLabel->getScale() + 9.0f,
                orbLabel->getPosition().y - 0.5f));

            m_dynamicObjects->addObject(orbIcon);
            m_dynamicObjects->addObject(orbLabel);
        }
    } else if (levelID == -2) {
        for (unsigned int i = 0; i < m_progressObjects->count(); ++i) {
            static_cast<cocos2d::CCNode*>(m_progressObjects->objectAtIndex(i))->setVisible(false);
        }

        cocos2d::CCSprite* door = cocos2d::CCSprite::createWithSpriteFrameName("theTowerDoor_001.png");
        CCMenuItemSpriteExtra* doorBtn = CCMenuItemSpriteExtra::create(door, nullptr, this, menu_selector(LevelPage::onTheTower));
        cocos2d::CCMenu* menu = cocos2d::CCMenu::createWithItem(doorBtn);
        this->addChild(menu);
        menu->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 5.0f));
        m_dynamicObjects->addObject(menu);

        cocos2d::CCSprite* label = cocos2d::CCSprite::createWithSpriteFrameName("theTowerLabel_001.png");
        door->addChild(label);
        label->setPosition(cocos2d::CCPoint(
            door->getContentSize().width * 0.5f,
            door->getContentSize().height + 23.0f));

        door->runAction(cocos2d::CCRepeatForever::create(cocos2d::CCSequence::create(
            cocos2d::CCEaseInOut::create(cocos2d::CCMoveBy::create(2.5f, cocos2d::CCPoint(0.0f, 5.0f)), 1.8f),
            cocos2d::CCEaseInOut::create(cocos2d::CCMoveBy::create(2.5f, cocos2d::CCPoint(0.0f, -5.0f)), 1.8f),
            nullptr)));
    } else {
        for (unsigned int i = 0; i < m_progressObjects->count(); ++i) {
            static_cast<cocos2d::CCNode*>(m_progressObjects->objectAtIndex(i))->setVisible(false);
        }

        cocos2d::CCLabelBMFont* label = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
        this->addChild(label);
        label->setPosition(cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 50.0f));
        m_dynamicObjects->addObject(label);

        addSecretDoor();
    }

    if (m_coinObject) {
        m_coinObject->setVisible(m_level->m_levelID.random - m_level->m_levelID.seed == -1);
    }

    m_isBusy = false;
}

LevelPage::~LevelPage() {
    if (m_level) {
        m_level->release();
    }
    if (m_progressObjects) {
        m_progressObjects->release();
    }
    if (m_levelObjects) {
        m_levelObjects->release();
    }
    if (m_coins) {
        m_coins->release();
    }
    if (m_dynamicObjects) {
        m_dynamicObjects->release();
    }
}