#include "Layers/SecretLayer5.h"
#include "UI/TextArea.h"
#include "Cocos/CCSpriteWithHue.h"
#include "Cocos/CCSpriteGrayscale.h"
#include "Sprites/LoadingCircleSprite.h"
#include "Layers/RewardUnlockLayer.h"
#include "Models/GJRewardItem.h"
#include "Layers/DialogLayer.h"
#include "Models/DialogObject.h"
#include "Managers/GameStatsManager.h"
#include "GameLevelManager.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Utils/PlatformToolbox.h"
#include "Managers/MusicDownloadManager.h"
#include "Audio/FMODAudioEngine.h"
#include "Layers/LevelAreaInnerLayer.h"
#include "Layers/LevelAreaLayer.h"

void SecretLayer5::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
}

void SecretLayer5::animateHead() {
    FMODAudioEngine* audioEngine = FMODAudioEngine::sharedEngine();
    MusicDownloadManager* musicManager = MusicDownloadManager::sharedState();
    std::string sfxPath = MusicDownloadManager::pathForSFX(musicManager, m_soundEffects[m_chatIndex]);
    FMODAudioEngine::playEffect(audioEngine, sfxPath, 1.0, 0.0, 0.7);

    cocos2d::CCActionTween* tween1 = cocos2d::CCActionTween::create(1.35, "hue", 0.0, 25.0);
    cocos2d::CCActionTween* tween2 = cocos2d::CCActionTween::create(1.35, "hue", 25.0, 0.0);
    cocos2d::CCEaseInOut* ease1 = cocos2d::CCEaseInOut::create(tween1, 2.0);
    cocos2d::CCDelayTime* delay1 = cocos2d::CCDelayTime::create(0.45);
    cocos2d::CCEaseInOut* ease2 = cocos2d::CCEaseInOut::create(tween2, 2.0);
    cocos2d::CCAction* seq1 = cocos2d::CCSequence::create(ease1, delay1, ease2, nullptr);
    cocos2d::CCNode::runAction(this, seq1);

    cocos2d::CCNode::stopAllActions(m_eyesSprite);
    m_eyesSprite->setOpacity(0);
    m_eyesSprite->setColor(ccc3(255, 255, 255));

    cocos2d::CCFadeTo* fade1 = cocos2d::CCFadeTo::create(1.35, 175);
    cocos2d::CCEaseInOut* ease3 = cocos2d::CCEaseInOut::create(fade1, 2.0);
    cocos2d::CCDelayTime* delay2 = cocos2d::CCDelayTime::create(0.45);
    cocos2d::CCFadeTo* fade2 = cocos2d::CCFadeTo::create(1.62, 0);
    cocos2d::CCEaseInOut* ease4 = cocos2d::CCEaseInOut::create(fade2, 2.0);
    cocos2d::CCAction* seq2 = cocos2d::CCSequence::create(ease3, delay2, ease4, nullptr);
    cocos2d::CCNode::runAction(m_eyesSprite, seq2);

    m_wraithGraySprite->setColor(ccc3(200, 200, 100));

    cocos2d::CCDelayTime* delay3 = cocos2d::CCDelayTime::create(0.72);
    cocos2d::CCFadeTo* fade3 = cocos2d::CCFadeTo::create(2.7, 255);
    cocos2d::CCEaseInOut* ease5 = cocos2d::CCEaseInOut::create(fade3, 1.6);
    cocos2d::CCAction* seq3 = cocos2d::CCSequence::create(delay3, ease5, nullptr);
    cocos2d::CCNode::runAction(m_wraithGraySprite, seq3);

    cocos2d::CCNode::stopAllActions(m_lockSprite);
    cocos2d::CCTintTo* tint1 = cocos2d::CCTintTo::create(1.35, 165, 71, 183);
    cocos2d::CCEaseInOut* ease6 = cocos2d::CCEaseInOut::create(tint1, 2.0);
    cocos2d::CCDelayTime* delay4 = cocos2d::CCDelayTime::create(0.045);
    cocos2d::CCTintTo* tint2 = cocos2d::CCTintTo::create(1.35, 49, 31, 68);
    cocos2d::CCEaseInOut* ease7 = cocos2d::CCEaseInOut::create(tint2, 2.0);
    cocos2d::CCAction* seq4 = cocos2d::CCSequence::create(ease6, delay4, ease7, nullptr);
    cocos2d::CCNode::runAction(m_lockSprite, seq4);

    cocos2d::CCTintTo* tint3 = cocos2d::CCTintTo::create(1.35, 50, 14, 50);
    cocos2d::CCEaseInOut* ease8 = cocos2d::CCEaseInOut::create(tint3, 2.0);
    cocos2d::CCTintTo* tint4 = cocos2d::CCTintTo::create(1.8, 23, 14, 47);
    cocos2d::CCEaseInOut* ease9 = cocos2d::CCEaseInOut::create(tint4, 2.0);
    cocos2d::CCAction* seq5 = cocos2d::CCSequence::create(ease8, ease9, nullptr);
    cocos2d::CCNode::runAction(m_background, seq5);
}

void SecretLayer5::claimOnlineReward() {
    GameStatsManager* statsManager = GameStatsManager::sharedState();
    bool isUnlocked = GameStatsManager::isSpecialChestUnlocked(statsManager, m_chestID);

    if (!isUnlocked) {
        GameStatsManager* statsManager2 = GameStatsManager::sharedState();
        GJRewardItem* rewardItem = GameStatsManager::unlockOnlineChest(statsManager2, m_chestID);
        if (!rewardItem) {
            unlockUI();
            return;
        }
        RewardUnlockLayer* layer = RewardUnlockLayer::create(rewardItem->m_rewardType, 0);
        cocos2d::CCNode* parent = getParent();
        parent->addChild(layer, getZOrder() + 1);
        RewardUnlockLayer::showCollectReward(layer, rewardItem);
        cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
            cocos2d::CCDelayTime::create(0.5),
            cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer5::unlockUI)),
            nullptr);
        cocos2d::CCNode::runAction(this, seq);
        return;
    }

    unlockUI();
}

SecretLayer5* SecretLayer5::create() {
    SecretLayer5* layer = new SecretLayer5();
    if (layer && layer->init()) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

void SecretLayer5::dialogClosed(DialogLayer* layer) {
}

void SecretLayer5::fadeInMessage() {
    cocos2d::ccArray* children = m_messageArea->m_label->getChildren()->data;
    if ((int)children->num > 0) {
        for (unsigned int i = 0; i < children->num; i++) {
            cocos2d::CCObject* obj = children->arr[i];
            cocos2d::CCArray* innerChildren = (cocos2d::CCArray*)obj->getUserObject();
            if (innerChildren && innerChildren->count() > 0) {
                for (unsigned int j = 0; j < innerChildren->count(); j++) {
                    cocos2d::CCNode* node = (cocos2d::CCNode*)innerChildren->objectAtIndex(j);
                    int randVal = rand();
                    ccColor3B color;
                    color.r = (unsigned char)((float)((float)randVal * 4.6566e-10f) * 25.0f + 150.0f);
                    color.g = 0;
                    color.b = 255;
                    node->setColor(color);
                }
            }
        }
    }

    float duration = TextArea::fadeInCharacters(m_messageArea, 0.6, 0.06, 0, TextArea::Scale);
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(duration + 0.6),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer5::fadeOutMessage)),
        nullptr);
    cocos2d::CCNode::runAction(this, seq);
}

void SecretLayer5::fadeInSubmitMessage() {
    std::string text = textForWraithID(m_soundEffects[m_chatIndex]);
    TextArea::setString(m_messageArea, text);
    fadeInMessage();
}

void SecretLayer5::fadeOutMessage() {
    TextArea::fadeInCharacters(m_messageArea, 0.6, 0.06, 1, TextArea::Fade);
}

void SecretLayer5::finishLoadingState() {
    m_loading = false;
    if (m_rewardStatus) {
        handleSecretResponse();
    }
}

std::string SecretLayer5::getMessage() {
    return " ";
}

void SecretLayer5::handleSecretResponse() {
    LoadingCircleSprite::fadeInCircle(m_circleSprite, 0, 0.5, 0.0);
    m_wraithButton->setEnabled(true);

    if (m_rewardStatus == 1) {
        showSuccessAnimation();
    } else {
        showFailAnimation();
        showTextInput();
    }
}

void SecretLayer5::hideTextInput() {
    m_textInput->setTouchEnabled(false);
    m_textInput->onClickTrackNode(false);
    m_textInput->setVisible(false);
    cocos2d::CCNode::stopAllActions(m_inputBackground);
    cocos2d::CCFadeTo* fade = cocos2d::CCFadeTo::create(0.5, 0);
    cocos2d::CCNode::runAction(m_inputBackground, fade);
}

void SecretLayer5::incrementChatIdx() {
    int oldValue = m_soundEffects[m_chatIndex];
    m_chatIndex++;

    if ((int)m_soundEffects.size() - 1 < m_chatIndex + 1) {
        std::shuffle(m_soundEffects.begin(), m_soundEffects.end(), std::default_random_engine(rand()));
        m_chatIndex = 0;
        if (oldValue == m_soundEffects[0]) {
            m_chatIndex = 1;
        }
    }
}

bool SecretLayer5::init() {
    if (!cocos2d::CCLayer::init()) {
        return false;
    }

    cocos2d::CCTextureCache::sharedTextureCache()->addImage("SecretSheet.png", false);
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("SecretSheet.plist");
    cocos2d::CCTextureCache::sharedTextureCache()->addImage("TowerSheet.png", false);
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("TowerSheet.plist");

    GameManager::sharedState()->fadeInMusic("secretLoop04.ogg");

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_soundEffects = {
        0x579B, 0x5850, 0x5842, 0x5843, 0x57BD, 0x57C7, 0x5839, 0x5838,
        0x57F1, 0x583B, 0x583C, 0x583D, 0x5840, 0x57D7, 0x580B, 0x5737, 0x574D
    };

    srand((unsigned int)time(nullptr));
    std::shuffle(m_soundEffects.begin(), m_soundEffects.end(), std::default_random_engine(rand()));

    m_background = cocos2d::CCSprite::create("GJ_gradientBG.png");
    m_background->setAnchorPoint(cocos2d::CCPoint(0, 0));
    addChild(m_background, -3);
    m_background->setScaleX((winSize.width + 10.0f) / m_background->getContentSize().width);
    m_background->setScaleY((winSize.height + 10.0f) / m_background->getContentSize().height);
    m_background->setPosition(cocos2d::CCPoint(-5, -5));
    m_background->setColor(ccc3(0x1D, 0x2F, 0x2F));

    cocos2d::CCRect rect = cocos2d::CCRect(0, 0, 40, 40);
    cocos2d::CCPoint center = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f + 55.0f);

    m_messageLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
    addChild(m_messageLabel);
    m_messageLabel->setPosition(center + cocos2d::CCPoint(0, 43));
    updateMessageLabel(getMessage());

    m_inputBackground = cocos2d::extension::CCScale9Sprite::create("square02b_small.png", rect);
    m_inputBackground->setOpacity(100);
    m_inputBackground->setContentSize(cocos2d::CCSize(200, 40));
    addChild(m_inputBackground, -2);
    m_inputBackground->setPosition(center);
    m_inputBackground->setColor(ccc3(0, 0, 0));

    m_textInput = CCTextInputNode::create(180.0f, 50.0f, "...", "Thonburi");
    addChild(m_textInput, 20);
    m_textInput->m_delegate = this;
    m_textInput->setPosition(center);
    m_textInput->setMaxLabelScale(0.7f);
    m_textInput->setMaxLabelWidth(180.0f);
    m_textInput->m_textField->setVisible(false);
    m_textInput->m_maxLabelLength = 100;
    m_textInput->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-!?:;)(/\\\"'`*=+-_%[]<>|@&^#{}%$~");

    updateSearchLabel("");

    cocos2d::CCSprite* backSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCMenuItemSpriteExtra* backButton = CCMenuItemSpriteExtra::create(backSprite, nullptr, menu_selector(SecretLayer5::onBack));
    backButton->setSizeMult(1.5f);
    cocos2d::CCMenu* menu = cocos2d::CCMenu::create(backButton, nullptr);
    addChild(menu, 1);
    menu->setPosition(cocos2d::CCPoint(
        cocos2d::CCDirector::sharedDirector()->getScreenLeft() + 24.0f,
        cocos2d::CCDirector::sharedDirector()->getScreenTop() - 23.0f));

    if (PlatformToolbox::isControllerConnected()) {
        GameToolbox::addBackButton(this, backButton);
    }

    m_wraithSprite = CCSpriteWithHue::createWithSpriteFrameName("GJ_secretLock4_001.png");
    m_wraithGraySprite = CCSpriteGrayscale::createWithSpriteFrameName("GJ_secretLock4_001.png");
    m_wraithGraySprite->setColor(ccc3(200, 200, 100));
    m_wraithSprite->addChild(m_wraithGraySprite, 1);
    m_wraithGraySprite->setOpacity(0);
    m_wraithGraySprite->setPosition(cocos2d::CCNode::convertToNodeSpace(m_wraithSprite, cocos2d::CCPoint(0, 0)));

    m_eyesSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_secretLock4_eyes_001.png");
    m_eyesSprite->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE});
    m_wraithSprite->addChild(m_eyesSprite, 2);
    m_eyesSprite->setPosition(cocos2d::CCNode::convertToNodeSpace(m_wraithSprite, cocos2d::CCPoint(0, 0)));
    m_eyesSprite->setOpacity(0);

    cocos2d::CCNode* lockNode = cocos2d::CCNode::create();
    m_lockSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_secretLock2_2_001.png");
    lockNode->addChild(m_lockSprite);
    m_wraithSprite->addChild(lockNode, -2);
    lockNode->setPosition(cocos2d::CCNode::convertToNodeSpace(m_wraithSprite, cocos2d::CCPoint(0, -30)));
    m_lockSprite->setPosition(cocos2d::CCPoint(0, 25));
    m_lockSprite->setColor(ccc3(0x47, 0xB7, 0x99));

    m_wraithSprite->setScale(1.2f);

    m_wraithButton = CCMenuItemSpriteExtra::create(m_wraithSprite, nullptr, menu_selector(SecretLayer5::onSubmit));
    menu->addChild(m_wraithButton);
    m_wraithButton->setPosition(cocos2d::CCNode::convertToNodeSpace(menu, cocos2d::CCPoint(winSize.width * 0.5f, winSize.height * 0.5f - 25.0f)));
    m_wraithButton->setSizeMult(0.7f);

    m_circleSprite = LoadingCircleSprite::create(0.5f);
    m_circleSprite->setScale(0.5f);
    addChild(m_circleSprite, 28);
    m_circleSprite->setOpacity(0);
    m_circleSprite->setPosition(center);
    m_circleSprite->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE});
    m_circleSprite->setColor(ccc3(40, 100, 255));

    GameManager::sharedState()->loadFont(19);
    std::string fontFile = GameManager::sharedState()->getFontFile(19);
    m_messageArea = TextArea::create(" ", fontFile, 1.0f, 533.33f, cocos2d::CCPoint(20.0f, 1.0f), 20.0f, false);
    m_messageArea->setAnchorPoint(cocos2d::CCPoint(0.5, 0.5));
    m_messageArea->setScale(0.75f);
    addChild(m_messageArea, 3);
    m_messageArea->setPosition(center + cocos2d::CCPoint(0, 60));

    m_torchFires = cocos2d::CCArray::create();
    m_torchFires->retain();

    for (int i = 0; i < 2; i++) {
        cocos2d::CCSprite* torch = cocos2d::CCSprite::createWithSpriteFrameName("towerTorchInner_001.png");
        torch->setColor(ccc3(0x58, 0x8B, 0x99));
        addChild(torch, 4);
        float xOffset = (i == 0) ? -195.0f : 195.0f;
        torch->setPosition(cocos2d::CCPoint(winSize.width * 0.5f + xOffset, winSize.height * 0.5f + 4.0f));
        cocos2d::CCPoint torchPos = torch->getPosition() + cocos2d::CCPoint(0, 33);
        LevelAreaLayer::addTorch(this, cocos2d::CCPoint(1, 1), torchPos, 0, 2, 4, 1, nullptr);
        cocos2d::CCObject* obj1 = torch->getChildren()->objectAtIndex(0);
        cocos2d::CCObject* obj2 = torch->getChildren()->objectAtIndex(1);
        m_torchFires->addObject(obj1);
        m_torchFires->addObject(obj2);
    }

    cocos2d::CCNode::scheduleUpdate(this);

    GameLevelManager::sharedState()->m_GJOnlineRewardDelegate = this;

    if (GameManager::sharedState()->getUGV("38")) {
        return true;
    }

    m_uiLocked = false;
    cocos2d::CCSequence* seq = cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(1.2),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer5::showFirstDialog)),
        nullptr);
    cocos2d::CCNode::runAction(this, seq);
    m_textInput->setTouchEnabled(false);

    return true;
}

void SecretLayer5::keyBackClicked() {
    onBack(nullptr);
}

void SecretLayer5::onBack(cocos2d::CCObject* sender) {
    if (m_uiLocked) {
        return;
    }

    m_textInput->onClickTrackNode(false);
    setKeypadEnabled(false);
    cocos2d::CCScene* scene = LevelAreaInnerLayer::scene(true);
    cocos2d::CCTransitionFade* transition = cocos2d::CCTransitionFade::create(0.5f, scene);
    cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
    m_exiting = true;
}

void SecretLayer5::onExit() {
    cocos2d::CCLayer::onExit();
    if (m_exiting) {
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("SecretSheet.plist");
        cocos2d::CCTextureCache::sharedTextureCache()->removeTextureForKey("SecretSheet.png");
    }
}

void SecretLayer5::onSubmit(cocos2d::CCObject* sender) {
    if (m_loading || m_uiLocked) {
        return;
    }

    m_rewardStatus = 0;
    m_chestID = "";
    m_loading = true;

    std::string inputText = m_textInput->getString();
    m_textInput->setString("");
    m_textInput->updateLabel(" ");

    MusicDownloadManager* musicManager = MusicDownloadManager::sharedState();
    int sfxID = ((float)rand() * 4.6566e-10f <= 0.5f) ? 9983 : 9982;
    std::string sfxPath = MusicDownloadManager::pathForSFX(musicManager, sfxID);
    FMODAudioEngine::sharedEngine()->playEffect(sfxPath, (float)((float)rand() * 4.6566e-10f) * 0.1f + 0.9f, 0.0f, 0.5f);

    cocos2d::CCParticleSystemQuad* particle = GameToolbox::particleFromString(
        "40a0.21a0.81a0.2a180a90a10a12a6a90a12a0a0a0a0a0a0a25a10a0a10a0a0a0.815686a0a1a0a1a0a4a1a0a0a0.733333a0a0a0a1a0a1a0"
        "a0a0a0.2a0a0a0a0a0a0a0a0a2a1a1a0a0a26a0a0a0a0a0a0a0a0a0a0a0a0a0a0",
        nullptr, false);
    addChild(particle, 30);
    particle->setPosition(m_textInput->getPosition() + cocos2d::CCPoint(0, 0));

    incrementChatIdx();

    cocos2d::CCNode::runAction(this, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.3),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer5::fadeInSubmitMessage)),
        nullptr));

    cocos2d::CCNode::runAction(this, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.0),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer5::animateHead)),
        nullptr));

    hideTextInput();
    LoadingCircleSprite::fadeInCircle(m_circleSprite, 0, 0.8, 150.0);
    m_wraithButton->setEnabled(false);

    GameLevelManager::sharedState()->getGJSecretReward(m_chestID);

    cocos2d::CCNode::runAction(this, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(4.0),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer5::finishLoadingState)),
        nullptr));
}

void SecretLayer5::onlineRewardStatusFailed() {
    m_rewardStatus = 2;
    m_chestID = "";
    if (!m_loading) {
        handleSecretResponse();
    }
}

void SecretLayer5::onlineRewardStatusFinished(std::string key) {
    GameStatsManager* statsManager = GameStatsManager::sharedState();
    GJRewardItem* rewardItem = GameStatsManager::getRewardForSpecialChest(statsManager, key);

    if (!rewardItem) {
        onlineRewardStatusFailed();
        return;
    }

    GameStatsManager* statsManager2 = GameStatsManager::sharedState();
    bool isUnlocked = GameStatsManager::isSpecialChestUnlocked(statsManager2, key);

    if (isUnlocked) {
        onlineRewardStatusFailed();
    } else {
        m_rewardStatus = 1;
        m_chestID = key;
        if (!m_loading) {
            handleSecretResponse();
        }
    }
}

void SecretLayer5::playWinSFX() {
    FMODAudioEngine* audioEngine = FMODAudioEngine::sharedEngine();
    MusicDownloadManager* musicManager = MusicDownloadManager::sharedState();
    std::string sfxPath = MusicDownloadManager::pathForSFX(musicManager, 20982);
    FMODAudioEngine::playEffect(audioEngine, sfxPath, 1.0, 0.0, 0.7);
}

cocos2d::CCScene* SecretLayer5::scene() {
    cocos2d::CCScene* scene = cocos2d::CCScene::create();
    SecretLayer5* layer = SecretLayer5::create();
    scene->addChild(layer);
    return scene;
}

void SecretLayer5::showDialog(int index) {
    cocos2d::CCArray* objects = cocos2d::CCArray::create();

    switch (index) {
    case 1:
        objects->addObject(DialogObject::create("The Gatekeeper", "This is random message 1", 3, 1.0f, true, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Gatekeeper", "Random message 1 end", 3, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 2:
        objects->addObject(DialogObject::create("The Gatekeeper", "This is random message 2", 3, 1.0f, true, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Gatekeeper", "Random message 2 end", 3, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 3:
        objects->addObject(DialogObject::create("The Gatekeeper", "This is random message 3", 3, 1.0f, true, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Gatekeeper", "Random message 3 end", 3, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 4:
        objects->addObject(DialogObject::create("The Gatekeeper", "This is random message 4", 3, 1.0f, true, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Gatekeeper", "Random message 4 end", 3, 1.0f, false, ccc3(255, 255, 255)));
        break;
    case 5:
        objects->addObject(DialogObject::create("The Gatekeeper", "This is random message 5", 3, 1.0f, true, ccc3(255, 255, 255)));
        objects->addObject(DialogObject::create("The Gatekeeper", "Random message 5 end", 3, 1.0f, false, ccc3(255, 255, 255)));
        break;
    default:
        return;
    }

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
    dialogLayer->m_delegate = this;
    addChild(dialogLayer, 100);
    dialogLayer->updateChatPlacement(DialogLayer::Center);
    dialogLayer->animateIn(DialogLayer::FromLeft);
}

void SecretLayer5::showFailAnimation() {
    m_wraithGraySprite->setColor(ccc3(255, 200, 100));
    cocos2d::CCNode::stopAllActions(m_wraithGraySprite);
    cocos2d::CCNode::runAction(m_wraithGraySprite, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.8),
        cocos2d::CCEaseInOut::create(cocos2d::CCFadeTo::create(1.8, 0), 2.0),
        nullptr));

    cocos2d::CCNode::stopAllActions(m_eyesSprite);
    m_eyesSprite->setOpacity(0);
    m_eyesSprite->setColor(ccc3(50, 50, 50));
    cocos2d::CCNode::runAction(m_eyesSprite, cocos2d::CCSequence::create(
        cocos2d::CCEaseInOut::create(cocos2d::CCFadeTo::create(0.01, 255), 2.0),
        cocos2d::CCDelayTime::create(0.2),
        cocos2d::CCEaseInOut::create(cocos2d::CCFadeTo::create(2.2, 0), 2.0),
        nullptr));

    cocos2d::CCNode::runAction(m_wraithSprite, cocos2d::CCSequence::create(
        cocos2d::CCMoveBy::create(0.0, cocos2d::CCPoint(3, 0)),
        cocos2d::CCMoveBy::create(0.0, cocos2d::CCPoint(-3, 0)),
        cocos2d::CCMoveBy::create(0.05, cocos2d::CCPoint(3, 0)),
        cocos2d::CCMoveBy::create(0.05, cocos2d::CCPoint(-3, 0)),
        cocos2d::CCMoveBy::create(0.06, cocos2d::CCPoint(2, 0)),
        cocos2d::CCMoveBy::create(0.06, cocos2d::CCPoint(-2, 0)),
        nullptr));

    FMODAudioEngine* audioEngine = FMODAudioEngine::sharedEngine();
    MusicDownloadManager* musicManager = MusicDownloadManager::sharedState();
    std::string sfxPath1 = MusicDownloadManager::pathForSFX(musicManager, 2306);
    FMODAudioEngine::playEffect(audioEngine, sfxPath1, 0.8f, 0.0f, 1.0f);

    FMODAudioEngine* audioEngine2 = FMODAudioEngine::sharedEngine();
    MusicDownloadManager* musicManager2 = MusicDownloadManager::sharedState();
    std::string sfxPath2 = MusicDownloadManager::pathForSFX(musicManager2, 12918);
    float pitch = (float)((float)rand() * 4.6566e-10f) * 0.3f + 0.8f;
    float pan = (float)((float)rand() * 4.6566e-10f) * 0.3f + 0.4f;
    FMODAudioEngine::playEffect(audioEngine2, sfxPath2, pitch, 0.0f, pan);

    cocos2d::CCNode* lockParent = m_lockSprite->getParent();
    cocos2d::CCActionInterval* rotate1 = cocos2d::CCEaseOut::create(cocos2d::CCRotateBy::create(0.125, 6.0), 1.8);
    cocos2d::CCActionInterval* rotate2 = cocos2d::CCEaseInOut::create(cocos2d::CCRotateBy::create(0.25, -12.0), 1.8);
    cocos2d::CCActionInterval* rotate3 = cocos2d::CCEaseInOut::create(cocos2d::CCRotateBy::create(0.275, 11.4), 1.8);
    cocos2d::CCActionInterval* rotate4 = cocos2d::CCEaseInOut::create(cocos2d::CCRotateBy::create(0.3, -10.2), 1.8);
    cocos2d::CCActionInterval* rotate5 = cocos2d::CCEaseInOut::create(cocos2d::CCRotateBy::create(0.35, 9.0), 1.8);
    cocos2d::CCActionInterval* rotate6 = cocos2d::CCEaseInOut::create(cocos2d::CCRotateBy::create(0.425, -7.2), 1.8);
    cocos2d::CCActionInterval* rotate7 = cocos2d::CCEaseInOut::create(cocos2d::CCRotateBy::create(0.275, 3.0), 2.0);
    cocos2d::CCNode::runAction(lockParent, cocos2d::CCSequence::create(
        rotate1, rotate2, rotate3, rotate4, rotate5, rotate6, rotate7, nullptr));

    m_lockSprite->setColor(ccc3(0x58, 0x1D, 0x3D));
    cocos2d::CCNode::runAction(m_lockSprite, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.6),
        cocos2d::CCEaseInOut::create(cocos2d::CCTintTo::create(2.2, 0x66, 0x47, 0x99), 1.8),
        nullptr));
}

void SecretLayer5::showFirstDialog() {
    GameManager::sharedState()->setUGV("38", true);

    cocos2d::CCArray* objects = cocos2d::CCArray::create();
    objects->addObject(DialogObject::create("The Wraith", "<cl>Great...</c><d020> Just what I needed.<d050>\nAnother visitor to my <cr>prison</c>.", 54, 1.0f, true, ccc3(255, 255, 255)));
    objects->addObject(DialogObject::create("The Wraith", "Must be my lucky day...<d040> <cg>or yours</c>.\n<d050><cl>I don't really have days anymore.</c>", 53, 1.0f, false, ccc3(255, 255, 255)));
    objects->addObject(DialogObject::create("The Wraith", "I wasn't always like this,<d015> you know.\n<d050>I used to be something,<d040> <co>greater</c><d010>.<d015>.<d020>.", 55, 1.0f, false, ccc3(255, 255, 255)));
    objects->addObject(DialogObject::create("The Wraith", "Now instead,<d030> I get to listen to you.\n<d050><cl>What a treat...</c>", 53, 1.0f, false, ccc3(255, 255, 255)));
    objects->addObject(DialogObject::create("The Wraith", "<cg>Go on</c>, <d030><co>or don't</c>.\n<d050>Not like I can stop you.", 56, 1.0f, false, ccc3(255, 255, 255)));

    DialogLayer* dialogLayer = DialogLayer::createWithObjects(objects, 2);
    addChild(dialogLayer, 100);
    dialogLayer->updateChatPlacement(DialogLayer::Center);
    dialogLayer->animateIn(DialogLayer::FromLeft);

    m_uiLocked = false;
    m_textInput->setTouchEnabled(true);
}

void SecretLayer5::showSuccessAnimation() {
    m_uiLocked = true;
    cocos2d::CCSize winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    m_wraithGraySprite->setColor(ccc3(175, 255, 200));
    cocos2d::CCNode::stopAllActions(m_wraithGraySprite);
    cocos2d::CCNode::runAction(m_wraithGraySprite, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.8),
        cocos2d::CCEaseInOut::create(cocos2d::CCFadeTo::create(1.8, 0), 2.0),
        nullptr));

    cocos2d::CCNode::stopAllActions(m_eyesSprite);
    m_eyesSprite->setOpacity(0);
    m_eyesSprite->setColor(ccc3(50, 255, 50));
    cocos2d::CCNode::runAction(m_eyesSprite, cocos2d::CCSequence::create(
        cocos2d::CCEaseInOut::create(cocos2d::CCFadeTo::create(0.01, 255), 2.0),
        cocos2d::CCDelayTime::create(0.2),
        cocos2d::CCEaseInOut::create(cocos2d::CCFadeTo::create(2.2, 0), 2.0),
        nullptr));

    cocos2d::CCNode::runAction(m_wraithSprite, cocos2d::CCSequence::create(
        cocos2d::CCMoveBy::create(0.0, cocos2d::CCPoint(5, 0)),
        cocos2d::CCMoveBy::create(0.0, cocos2d::CCPoint(-5, 0)),
        cocos2d::CCMoveBy::create(0.05, cocos2d::CCPoint(5, 0)),
        cocos2d::CCMoveBy::create(0.05, cocos2d::CCPoint(-5, 0)),
        cocos2d::CCMoveBy::create(0.06, cocos2d::CCPoint(2, 0)),
        cocos2d::CCMoveBy::create(0.06, cocos2d::CCPoint(-2, 0)),
        nullptr));

    cocos2d::CCNode::runAction(m_wraithSprite, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.05),
        cocos2d::CCEaseOut::create(cocos2d::CCScaleTo::create(0.2, 1.5), 1.8),
        cocos2d::CCDelayTime::create(0.15),
        cocos2d::CCEaseInOut::create(cocos2d::CCScaleTo::create(1.2, 1.2), 2.0),
        nullptr));

    cocos2d::CCPoint pos1 = cocos2d::CCPoint(winSize.width * 0.5f, winSize.height + 10.0f);
    cocos2d::CCPoint pos2 = cocos2d::CCPoint(0, -25.0f - (winSize.height * 0.5f));
    cocos2d::CCLightning* lightning = cocos2d::CCLightning::lightningWithStrikePoint(pos2, 0.5f);
    lightning->setPosition(pos1);
    lightning->setColor(ccc3(255, 255, 255));
    lightning->m_unknown5[0] = 1;
    lightning->m_unknown5[1] = 1;
    lightning->m_fRotationY = 1.0f;
    lightning->m_uOrderOfArrival = 5.0f;
    addChild(lightning, 0);

    cocos2d::CCLayerColor* layerColor = cocos2d::CCLayerColor::create();
    layerColor->setBlendFunc(cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE});
    addChild(layerColor, 10);
    cocos2d::CCNode::runAction(layerColor, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.08),
        cocos2d::CCBlink::create(0.12, 1),
        cocos2d::CCCallFunc::create(layerColor, callfunc_selector(cocos2d::CCNode::removeFromParent)),
        nullptr));

    cocos2d::CCNode::runAction(m_background, cocos2d::CCSequence::create(
        cocos2d::CCEaseInOut::create(cocos2d::CCTintTo::create(0.3, 23, 42, 47), 2.0),
        cocos2d::CCDelayTime::create(0.1),
        cocos2d::CCEaseInOut::create(cocos2d::CCTintTo::create(0.8, 23, 14, 47), 2.0),
        nullptr));

    FMODAudioEngine* audioEngine = FMODAudioEngine::sharedEngine();
    MusicDownloadManager* musicManager = MusicDownloadManager::sharedState();
    std::string sfxPath1 = MusicDownloadManager::pathForSFX(musicManager, 1254);
    FMODAudioEngine::playEffect(audioEngine, sfxPath1, 0.8f, 0.0f, 1.0f);

    FMODAudioEngine* audioEngine2 = FMODAudioEngine::sharedEngine();
    MusicDownloadManager* musicManager2 = MusicDownloadManager::sharedState();
    std::string sfxPath2 = MusicDownloadManager::pathForSFX(musicManager2, 7636);
    FMODAudioEngine::playEffect(audioEngine2, sfxPath2, (float)((float)rand() * 4.6566e-10f) * 0.2f + 0.9f, 0.0f, 0.8f);

    FMODAudioEngine* audioEngine3 = FMODAudioEngine::sharedEngine();
    MusicDownloadManager* musicManager3 = MusicDownloadManager::sharedState();
    std::string sfxPath3 = MusicDownloadManager::pathForSFX(musicManager3, 1277);
    FMODAudioEngine::playEffect(audioEngine3, sfxPath3, (float)((float)rand() * 4.6566e-10f) * 0.3f + 0.8f, 0.0f, 1.0f);

    FMODAudioEngine* audioEngine4 = FMODAudioEngine::sharedEngine();
    MusicDownloadManager* musicManager4 = MusicDownloadManager::sharedState();
    std::string sfxPath4 = MusicDownloadManager::pathForSFX(musicManager4, 20986);
    FMODAudioEngine::playEffect(audioEngine4, sfxPath4, (float)((float)rand() * 4.6566e-10f) * 0.3f + 0.8f, 0.0f, 1.0f);

    m_lockSprite->setColor(ccc3(0x2C, 0x38, 0x3D));
    cocos2d::CCNode::runAction(m_lockSprite, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.6),
        cocos2d::CCEaseInOut::create(cocos2d::CCTintTo::create(2.2, 0x66, 0x47, 0x99), 1.8),
        nullptr));

    for (unsigned int i = 0; i < m_torchFires->count(); i++) {
        cocos2d::CCSprite* torch = (cocos2d::CCSprite*)m_torchFires->objectAtIndex(i);
        ccColor3B originalColor = torch->getColor();
        unsigned char r = (i & 1) ? 150 : 0;
        cocos2d::CCNode::runAction(torch, cocos2d::CCSequence::create(
            cocos2d::CCEaseInOut::create(cocos2d::CCTintTo::create(0.2, r, 255, 0), 2.0),
            cocos2d::CCTintTo::create(1.2, originalColor.r, originalColor.g, originalColor.b),
            nullptr));
    }

    cocos2d::CCNode::runAction(this, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(0.3),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer5::playWinSFX)),
        nullptr));

    cocos2d::CCNode::runAction(this, cocos2d::CCSequence::create(
        cocos2d::CCDelayTime::create(1.0),
        cocos2d::CCCallFunc::create(this, callfunc_selector(SecretLayer5::claimOnlineReward)),
        nullptr));
}

void SecretLayer5::showTextInput() {
    m_textInput->setTouchEnabled(true);
    m_textInput->setVisible(true);
    cocos2d::CCNode::stopAllActions(m_inputBackground);
    cocos2d::CCNode::runAction(m_inputBackground, cocos2d::CCFadeTo::create(1.2, 100));
}

void SecretLayer5::textChanged(CCTextInputNode* node) {
    std::string text = node->getString();
    updateSearchLabel(text.c_str());
}

void SecretLayer5::textInputClosed(CCTextInputNode* node) {
    std::string text = node->getString();
    updateSearchLabel(text.c_str());
}

void SecretLayer5::textInputOpened(CCTextInputNode* node) {
    std::string text = node->getString();
    if (text == "") {
        m_textInput->updateLabel(" ");
    }
}

void SecretLayer5::unlockUI() {
    m_uiLocked = false;
    showTextInput();
}

void SecretLayer5::updateMessageLabel(std::string text) {
    m_messageLabel->setString(text.c_str());
    m_messageLabel->limitLabelWidth(320.0f, 0.6f, 0.0f);
}

void SecretLayer5::updateSearchLabel(const char* text) {
    cocos2d::CCLabelBMFont* textLabel = m_textInput->m_textLabel;
    bool isEmpty = (std::string(text) == "");
    bool isEllipsis = (std::string(text) == "...");

    if (isEmpty || isEllipsis) {
        m_textInput->updateLabel("...");
        textLabel->setColor(ccc3(255, 255, 255));
    } else {
        textLabel->setColor(ccc3(255, 255, 255));
    }
}

void SecretLayer5::updateTweenActionInt(float value, int tag) {
    if (tag == 1) {
        m_wraithSprite->setHueDegrees(value);
    }
}

SecretLayer5::~SecretLayer5() {
}