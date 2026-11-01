// EndLevelLayer.cpp
#include "Layers/EndLevelLayer.h"
#include "Layers/PlayLayer.h"
#include "Models/GJGameLevel.h"
#include "Managers/GameManager.h"
#include "Audio/FMODAudioEngine.h"
#include "Layers/CurrencyRewardLayer.h"
#include "Layers/LevelLeaderboard.h"
#include "Layers/GJGameLoadingLayer.h"
#include "UI/TextArea.h"
#include "Cocos/CCCircleWave.h"
#include "Managers/GameStatsManager.h"
#include "Utils/PlatformToolbox.h"

EndLevelLayer* EndLevelLayer::create(PlayLayer* playLayer) {
    EndLevelLayer* instance = new EndLevelLayer();
    if (instance && instance->init(playLayer)) {
        instance->autorelease();
        return instance;
    }
    CC_SAFE_DELETE(instance);
    return nullptr;
}

bool EndLevelLayer::init(PlayLayer* playLayer) {
    m_playLayer = playLayer;
    return GJDropDownLayer::init(" ", 230.0f, true);
}

EndLevelLayer::~EndLevelLayer() {
    CC_SAFE_RELEASE(m_coinsToAnimate);
}

void EndLevelLayer::customSetup() {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    GJGameLevel* level = m_playLayer->m_level;
    bool isPlatformer = m_playLayer->m_isPlatformer;

    // Calculate currency values
    if (m_playLayer->m_endLayerStars) {
        if (level->m_levelLength == 5) {
            m_moons = level->m_stars.random - level->m_stars.seed;
        } else {
            m_stars = level->m_stars.random - level->m_stars.seed;
        }
    }

    m_orbs = m_playLayer->m_orbs;
    m_diamonds = m_playLayer->m_diamonds;
    m_secretKey = m_playLayer->m_secretKey;

    // Position currencies
    int currencyCount = (m_stars > 0 || m_moons > 0) + (m_orbs > 0) + (m_diamonds > 0);
    float spacingY = (currencyCount > 2) ? -34.0f : -38.0f;
    float offsetY = (currencyCount == 2) ? 5.0f : 0.0f;

    CCPoint currencyPos = ccp(winSize.width * 0.5f + 120.0f, winSize.height * 0.5f + 18.0f + offsetY);

    if (m_stars > 0 || m_moons > 0) {
        m_starsPosition = currencyPos;
        currencyPos.y += spacingY;
    }

    if (m_orbs > 0) {
        m_orbsPosition = currencyPos;
        currencyPos.y += spacingY;
    }

    if (m_diamonds > 0) {
        m_diamondsPosition = currencyPos;
        currencyPos.y += spacingY;
    }

    // Level complete sprite
    const char* spriteName = m_playLayer->m_isPracticeMode ?
        "GJ_practiceComplete_001.png" : "GJ_levelComplete_001.png";

    CCSprite* completeSprite = CCSprite::createWithSpriteFrameName(spriteName);
    completeSprite->setScale(0.8f);
    m_mainLayer->addChild(completeSprite, 15);
    completeSprite->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f + 75.0f));

    if (GameManager::sharedState()->getGameVariable("0095")) {
        completeSprite->setFlipX(true);
    }

    m_buttonMenu->setVisible(false);
    m_endPosition = completeSprite->getPosition() + ccp(0.0f, 5.0f);

    // Stats labels
    CCPoint labelPos = completeSprite->getPosition() + ccp(0.0f, -40.0f);

    if (!isPlatformer) {
        // Attempts
        CCLabelBMFont* attemptsLabel = CCLabelBMFont::create(
            CCString::createWithFormat("Attempts: %i", m_playLayer->m_attempts)->getCString());
        m_mainLayer->addChild(attemptsLabel, 3);
        attemptsLabel->setPosition(labelPos);
        attemptsLabel->setScale(0.8f);
        labelPos.y -= 24.0f;

        // Jumps
        CCLabelBMFont* jumpsLabel = CCLabelBMFont::create(
            CCString::createWithFormat("Jumps: %i", m_playLayer->m_jumps)->getCString());
        m_mainLayer->addChild(jumpsLabel, 3);
        jumpsLabel->setPosition(labelPos);
        jumpsLabel->setScale(0.8f);
        labelPos.y -= 24.0f;
    }

    // Time label
    std::string timeStr;
    if (!isPlatformer) {
        int totalSeconds = (int)m_playLayer->m_gameState.m_unkDouble3;
        int hours = totalSeconds / 3600;
        int minutes = totalSeconds / 60 % 60;
        int seconds = totalSeconds % 60;

        if (hours > 0) {
            timeStr = CCString::createWithFormat("%02d:%02d:%02d", hours, minutes, seconds)->getCString();
        } else {
            timeStr = CCString::createWithFormat("%02d:%02d", minutes, seconds)->getCString();
        }
    } else {
        int playTime = GJBaseGameLayer::getPlayTimerMilli(m_playLayer);
        int hours = playTime / 1000 / 3600;
        int minutes = playTime / 1000 / 60 % 60;
        int seconds = playTime / 1000 % 60;
        int millis = playTime % 1000;

        if (hours > 0) {
            timeStr = CCString::createWithFormat("%02d:%02d:%02d", hours, minutes, seconds)->getCString();
        } else if (minutes > 0) {
            timeStr = CCString::createWithFormat("%i:%02d", seconds, millis)->getCString();
        } else {
            timeStr = CCString::createWithFormat("%s.%03d", seconds, millis)->getCString();
        }
    }

    CCLabelBMFont* timeLabel = CCLabelBMFont::create(
        CCString::createWithFormat("Time: %s", timeStr.c_str())->getCString());
    m_mainLayer->addChild(timeLabel, 3);
    timeLabel->setPosition(labelPos);
    timeLabel->limitLabelWidth(150.0f, 0.8f, 0.0f);
    labelPos.y -= 24.0f;

    // Points (platformer)
    if (isPlatformer) {
        int points = m_playLayer->m_gameState.m_points;
        if (points > 0) {
            CCLabelBMFont* pointsLabel = CCLabelBMFont::create(
                CCString::createWithFormat("Points: %s", GameToolbox::pointsToString(points).c_str())->getCString());
            m_mainLayer->addChild(pointsLabel, 3);
            pointsLabel->setPosition(labelPos);
            pointsLabel->limitLabelWidth(150.0f, 0.8f, 0.0f);
            labelPos.y -= 24.0f;
        }
    }

    // Editor verification text
    if (level->m_levelType == Editor) {
        TextArea* textArea = TextArea::create(" ", "bigFont.fnt", 1.0f, 620.0f, ccp(20.0f, 0.0f), 0.5f, true);
        textArea->setAnchorPoint(ccp(0.5f, 1.0f));
        m_mainLayer->addChild(textArea);
        textArea->setScale(0.55f);
        textArea->m_height = 34.0f;
        textArea->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f - 51.0f));

        if (isPlatformer) {
            textArea->setPosition(textArea->getPosition() + ccp(0.0f, -12.0f));
        }

        if (m_playLayer->m_isIgnoreDamageEnabled) {
            textArea->setString("You cannot verify a level after using Ignore Damage.");
        } else if (m_playLayer->m_isTestMode) {
            textArea->setString("You cannot verify a level if it has a start pos.");
        } else if (!m_playLayer->m_isPracticeMode || level->m_isVerified.random - level->m_isVerified.seed == 1) {
            if (GJGameLevel::areCoinsVerified(level)) {
                if (level->m_coins > 0) {
                    textArea->setString("Level and Coins Verified!");
                } else {
                    textArea->setString("Level Verified!");
                }
            } else {
                textArea->setString(getCoinString());
            }
        } else {
            textArea->setString("Complete the level in normal mode to verify it!");
        }
    } else if (m_playLayer->m_isPracticeMode) {
        TextArea* textArea = TextArea::create(" ", "bigFont.fnt", 1.0f, 620.0f, ccp(20.0f, 0.0f), 0.5f, true);
        textArea->setAnchorPoint(ccp(0.5f, 1.0f));
        m_mainLayer->addChild(textArea);
        textArea->setScale(0.55f);
        textArea->m_height = 34.0f;
        textArea->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f - 51.0f));
        textArea->setString("Well done... Now try to complete it without any checkpoints!");
    } else if (level->m_coins <= 0) {
        CCLabelBMFont* endText = CCLabelBMFont::create(getEndText());
        endText->limitLabelWidth(260.0f, 0.9f, 0.0f);
        m_mainLayer->addChild(endText, 3);
        endText->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f - 65.0f));
    }

    // Side menu buttons
    m_sideMenu = CCMenu::create();
    m_mainLayer->addChild(m_sideMenu, 1);

    CCPoint menuPos = ccp(winSize.width * 0.5f, winSize.height * 0.5f - 115.0f - 10.0f);
    menuPos = m_mainLayer->convertToWorldSpace(menuPos);
    menuPos = m_sideMenu->convertToNodeSpace(menuPos);

    // Replay button
    CCSprite* replaySprite = CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
    CCMenuItemSpriteExtra* replayBtn = CCMenuItemSpriteExtra::create(replaySprite, this, menu_selector(EndLevelLayer::onReplay));
    m_sideMenu->addChild(replayBtn);
    replayBtn->setPosition(menuPos + ccp(-100.0f, 0.0f));

    // Menu button
    CCSprite* menuSprite = CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
    CCMenuItemSpriteExtra* menuBtn = CCMenuItemSpriteExtra::create(menuSprite, this, menu_selector(EndLevelLayer::onMenu));
    m_sideMenu->addChild(menuBtn);
    menuBtn->setPosition(menuPos + ccp(100.0f, 0.0f));

    // Edit button (editor levels only)
    if (level->m_levelType == Editor) {
        CCSprite* editSprite = CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
        float scale = menuSprite->getContentSize().width / editSprite->getContentSize().width;
        editSprite->setScale(scale);
        CCMenuItemSpriteExtra* editBtn = CCMenuItemSpriteExtra::create(editSprite, this, menu_selector(EndLevelLayer::onEdit));
        m_sideMenu->addChild(editBtn);

        CCPoint editPos = m_mainLayer->convertToWorldSpace(ccp(winSize.width * 0.5f, winSize.height * 0.5f - 115.0f - 10.0f));
        editPos = m_sideMenu->convertToNodeSpace(editPos);
        editBtn->setPosition(editPos);
    }

    // Restart from checkpoint button (practice mode only)
    if (m_playLayer->m_isPracticeMode) {
        CCSprite* checkpointSprite = CCSprite::createWithSpriteFrameName("GJ_restartCheckBtn_001.png");
        CCMenuItemSpriteExtra* checkpointBtn = CCMenuItemSpriteExtra::create(checkpointSprite, this, menu_selector(EndLevelLayer::onRestartCheckpoint));
        m_sideMenu->addChild(checkpointBtn);

        CCPoint cpPos = m_mainLayer->convertToWorldSpace(ccp(winSize.width * 0.5f - 180.0f, winSize.height * 0.5f - 115.0f - 10.0f));
        cpPos = m_sideMenu->convertToNodeSpace(cpPos);
        checkpointBtn->setPosition(cpPos);
    }

    // Leaderboard button
    if ((isPlatformer || level->m_levelType != Editor) && level->m_levelType != Main) {
        CCSprite* leaderboardSprite = CCSprite::createWithSpriteFrameName("GJ_levelLeaderboardBtn_001.png");
        CCMenuItemSpriteExtra* leaderboardBtn = CCMenuItemSpriteExtra::create(leaderboardSprite, this, menu_selector(EndLevelLayer::onLevelLeaderboard));
        m_sideMenu->addChild(leaderboardBtn);

        CCPoint lbPos = m_mainLayer->convertToWorldSpace(ccp(winSize.width * 0.5f + 178.0f, winSize.height * 0.5f + 115.0f));
        lbPos = m_sideMenu->convertToNodeSpace(lbPos);
        leaderboardBtn->setPosition(lbPos);
    }

    // Hide button
    CCSprite* hideSprite = CCSprite::createWithSpriteFrameName("hideBtn_001.png");
    hideSprite->setOpacity(0);
    CCMenuItemSpriteExtra* hideBtn = CCMenuItemSpriteExtra::create(hideSprite, this, menu_selector(EndLevelLayer::onHideLayer));
    CCMenu* hideMenu = CCMenu::createWithItem(hideBtn);
    addChild(hideMenu, 100);
    hideMenu->setPosition(ccp(CCDirector::sharedDirector()->getScreenRight() - 17.0f,
                              CCDirector::sharedDirector()->getScreenTop() - 15.0f));

    hideSprite->runAction(CCFadeTo::create(1.0f, 100));

    // Coins
    bool isEditor = (level->m_levelType == Editor);
    m_notLocal = (level->m_levelType != Main);
    m_coinsVerified = (level->m_coinsVerified.random - level->m_coinsVerified.seed == 1);

    if (level->m_coins > 0 && !m_playLayer->m_isPracticeMode && !isEditor) {
        CCArray* coinSprites = CCArray::create();
        m_coinsToAnimate = coinSprites;
        m_coinsToAnimate->retain();

        CCPoint coinPos = ccp(winSize.width * 0.5f, winSize.height * 0.5f + 126.0f);
        coinPos = ccp(winSize.width * 0.5f, winSize.height * 0.5f - 64.0f);

        float spacing = (level->m_coins == 4) ? 15.0f : (level->m_coins > 4 ? 13.0f : 20.0f);

        for (int i = 1; i <= level->m_coins; i++) {
            const char* frame = m_notLocal ? "secretCoin_2_b_01_001.png" : "secretCoin_b_01_001.png";
            CCSprite* coin = CCSprite::createWithSpriteFrameName(frame);
            m_mainLayer->addChild(coin, 8);
            coinSprites->addObject(coin);

            if (m_notLocal && !m_coinsVerified) {
                coin->setColor(ccc3(75, 175, 255));
            }
        }

        GameToolbox::alignItemsHorisontally(coinSprites, spacing, coinPos, true);

        for (int i = 1; i <= level->m_coins; i++) {
            CCSprite* coin = (CCSprite*)coinSprites->objectAtIndex(i - 1);
            bool hasCoin = false;

            if (m_notLocal) {
                std::string key = GJGameLevel::getCoinKey(level, i);
                hasCoin = m_coinsVerified ?
                    GameStatsManager::sharedState()->hasUserCoin(key) :
                    GameStatsManager::sharedState()->hasPendingUserCoin(key);
            } else {
                std::string key = GJGameLevel::getCoinKey(level, i);
                hasCoin = GameStatsManager::sharedState()->hasSecretCoin(key);
            }

            bool hasItem = GJBaseGameLayer::hasItem(m_playLayer, i);

            if (hasCoin) {
                const char* uiFrame = m_notLocal ? "secretCoinUI2_001.png" : "secretCoinUI_001.png";
                CCSprite* uiCoin = CCSprite::createWithSpriteFrameName(uiFrame);
                uiCoin->setPosition(coin->getPosition());
                m_mainLayer->addChild(uiCoin, 9);

                if (m_notLocal && !m_coinsVerified) {
                    uiCoin->setColor(ccc3(75, 175, 255));
                }
            }

            if (hasItem) {
                const char* uiFrame = m_notLocal ? "secretCoinUI2_001.png" : "secretCoinUI_001.png";
                CCSprite* uiCoin = CCSprite::createWithSpriteFrameName(uiFrame);
                uiCoin->setPosition(coin->getPosition());
                m_coinsToAnimate->addObject(uiCoin);
                m_animateCoins = true;

                if (m_notLocal && !m_coinsVerified) {
                    uiCoin->setColor(ccc3(75, 175, 255));
                }
            }
        }
    }

    // Rate dialog
    PlatformToolbox::tryShowRateDialog("Geometry Dash");

    // Controller button hints
    if (PlatformToolbox::isControllerConnected()) {
        CCSprite* startHint = CCSprite::createWithSpriteFrameName("controllerBtn_Start_001.png");
        m_mainLayer->addChild(startHint, 10);
        startHint->setPosition(m_mainLayer->convertToNodeSpace(
            replayBtn->getParent()->convertToWorldSpace(replayBtn->getPosition())) + ccp(0.0f, -25.0f));

        CCSprite* bHint = CCSprite::createWithSpriteFrameName("controllerBtn_B_001.png");
        bHint->setScale(0.8f);
        m_mainLayer->addChild(bHint, 10);
        bHint->setPosition(m_mainLayer->convertToNodeSpace(
            menuBtn->getParent()->convertToWorldSpace(menuBtn->getPosition())) + ccp(0.0f, -25.0f));
    }
}

void EndLevelLayer::showLayer(bool instant) {
    if (GameManager::sharedState()->getGameVariable("0168")) {
        m_mainLayer->stopAllActions();
        layerVisible();
        m_mainLayer->setPosition(m_endPosition);
        setOpacity(100);
        enterAnimFinished();
        playStarEffect(0.0f);
        playCoinEffect(0.0f);
        playCurrencyEffect(0.0f);
        playDiamondEffect(0.0f);
        return;
    }

    m_mainLayer->stopAllActions();
    layerVisible();

    if (instant) {
        m_mainLayer->setPosition(m_endPosition);
        setOpacity(100);
        enterAnimFinished();
        playStarEffect(0.0f);
        playCoinEffect(0.0f);
        playCurrencyEffect(0.0f);
        playDiamondEffect(0.0f);
        return;
    }

    CCMoveTo* move = CCMoveTo::create(1.0f, m_endPosition);
    CCEaseBounceOut* ease = CCEaseBounceOut::create(move);
    CCSequence* seq = CCSequence::create(ease, CCCallFunc::create(this, callfunc_selector(EndLevelLayer::enterAnimFinished)), nullptr);
    m_mainLayer->runAction(seq);

    setOpacity(0);
    runAction(CCFadeTo::create(1.0f, 100));

    float delay = 0.7f;
    playCoinEffect(delay);

    if (m_coinsToAnimate) {
        delay += m_coinsToAnimate->count() * 0.35f;
    }

    playStarEffect(delay);
    playCurrencyEffect(delay);
    playDiamondEffect(delay);
}

void EndLevelLayer::enterAnimFinished() {}

void EndLevelLayer::playStarEffect(float duration) {
    if (!m_playLayer || !m_playLayer->m_endLayerStars) return;

    m_moons = (m_playLayer->m_level->m_levelLength == 5) ?
        m_playLayer->m_level->m_stars.random - m_playLayer->m_level->m_stars.seed :
        m_stars;

    CCNode* node = CCNode::create();
    m_mainLayer->addChild(node, 10);

    const char* frame = (m_playLayer->m_level->m_levelLength == 5) ?
        "GJ_bigMoon_001.png" : "GJ_bigStar_001.png";

    CCSprite* sprite = CCSprite::createWithSpriteFrameName(frame);
    sprite->setScale(0.8f);
    sprite->setPosition(ccp(15.0f, 0.0f));
    node->addChild(sprite);

    CCLabelBMFont* label = CCLabelBMFont::create(
        CCString::createWithFormat("+%i", m_moons)->getCString());
    label->setAnchorPoint(ccp(1.0f, 0.5f));
    label->setPosition(ccp(-2.0f, -16.0f));
    label->setScale(0.7f);
    label->m_eObjType = 4;
    label->limitLabelWidth(40.0f, 0.7f, 0.0f);
    node->addChild(label);

    node->setPosition(m_starsPosition);
    node->setScale(3.0f);

    CCCallFunc* call = CCCallFunc::create(this, callfunc_selector(EndLevelLayer::starEnterFinished));
    CCScaleTo* scaleTo = CCScaleTo::create(0.3f, 1.0f);
    CCEaseBounceOut* ease = CCEaseBounceOut::create(scaleTo);

    node->runAction(CCSequence::create(CCDelayTime::create(duration), ease, nullptr));
    node->runAction(CCSequence::create(CCDelayTime::create(duration + 0.2f), call, nullptr));

    sprite->setOpacity(0);
    label->setOpacity(0);
    sprite->runAction(CCSequence::create(CCDelayTime::create(duration), CCFadeIn::create(0.3f), nullptr));
    label->runAction(CCSequence::create(CCDelayTime::create(duration), CCFadeIn::create(0.3f), nullptr));
}

void EndLevelLayer::starEnterFinished() {
    if (!m_playLayer) return;

    bool isMoon = (m_playLayer->m_level->m_levelLength == 5);

    CCPoint pos = m_starsPosition + ccp(0.0f, 8.0f);

    CCParticleSystemQuad* particles = CCParticleSystemQuad::create("starEffect.plist");
    particles->onEnter();
    m_mainLayer->addChild(particles, 9);
    particles->setPosition(pos);

    CCCircleWave* wave = CCCircleWave::create(10.0f, 90.0f, 0.4f, false);
    wave->setColor(isMoon ? ccc3(255, 255, 0) : ccc3(0, 255, 255));
    wave->setPosition(pos);
    m_mainLayer->addChild(wave, 9);

    playEndEffect();
}

void EndLevelLayer::playCurrencyEffect(float duration) {
    if (!m_playLayer || m_orbs <= 0) return;

    CCNode* node = CCNode::create();
    m_mainLayer->addChild(node, 10);

    CCSprite* sprite = CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
    sprite->setScale(1.25f);
    sprite->setPosition(ccp(15.0f, 0.0f));
    node->addChild(sprite);

    CCLabelBMFont* label = CCLabelBMFont::create(
        CCString::createWithFormat("+%i", m_orbs)->getCString());
    label->setAnchorPoint(ccp(1.0f, 0.5f));
    label->setPosition(sprite->getPosition() + ccp(-20.0f, 0.0f));
    label->setScale(0.7f);
    label->m_eObjType = 4;
    label->limitLabelWidth(40.0f, 0.7f, 0.0f);
    node->addChild(label);

    node->setPosition(m_orbsPosition);
    node->setScale(3.0f);

    CCCallFunc* call = CCCallFunc::create(this, callfunc_selector(EndLevelLayer::currencyEnterFinished));
    CCScaleTo* scaleTo = CCScaleTo::create(0.3f, 1.0f);
    CCEaseBounceOut* ease = CCEaseBounceOut::create(scaleTo);

    node->runAction(CCSequence::create(CCDelayTime::create(duration), ease, nullptr));
    node->runAction(CCSequence::create(CCDelayTime::create(duration + 0.2f), call, nullptr));

    sprite->setOpacity(0);
    label->setOpacity(0);
    sprite->runAction(CCSequence::create(CCDelayTime::create(duration), CCFadeIn::create(0.3f), nullptr));
    label->runAction(CCSequence::create(CCDelayTime::create(duration), CCFadeIn::create(0.3f), nullptr));
}

void EndLevelLayer::currencyEnterFinished() {
    CCPoint pos = m_orbsPosition + ccp(0.0f, 8.0f);

    CCParticleSystemQuad* particles = CCParticleSystemQuad::create("starEffect.plist");
    particles->onEnter();
    m_mainLayer->addChild(particles, 9);
    particles->setPosition(pos);

    CCCircleWave* wave = CCCircleWave::create(10.0f, 90.0f, 0.4f, false);
    wave->setColor(ccc3(255, 255, 0));
    wave->setPosition(pos);
    m_mainLayer->addChild(wave, 9);

    playEndEffect();
}

void EndLevelLayer::playDiamondEffect(float duration) {
    if (!m_playLayer || m_diamonds <= 0) return;

    CCNode* node = CCNode::create();
    m_mainLayer->addChild(node, 10);

    CCSprite* sprite = CCSprite::createWithSpriteFrameName("GJ_bigDiamond_001.png");
    sprite->setScale(0.7f);
    sprite->setPosition(ccp(15.0f, 0.0f));
    node->addChild(sprite);

    CCLabelBMFont* label = CCLabelBMFont::create(
        CCString::createWithFormat("+%i", m_diamonds)->getCString());
    label->setAnchorPoint(ccp(1.0f, 0.5f));
    label->setPosition(sprite->getPosition() + ccp(-20.0f, 0.0f));
    label->setScale(0.7f);
    label->m_eObjType = 4;
    label->limitLabelWidth(40.0f, 0.7f, 0.0f);
    node->addChild(label);

    node->setPosition(m_diamondsPosition);
    node->setScale(3.0f);

    CCCallFunc* call = CCCallFunc::create(this, callfunc_selector(EndLevelLayer::diamondEnterFinished));
    CCScaleTo* scaleTo = CCScaleTo::create(0.3f, 1.0f);
    CCEaseBounceOut* ease = CCEaseBounceOut::create(scaleTo);

    node->runAction(CCSequence::create(CCDelayTime::create(duration), ease, nullptr));
    node->runAction(CCSequence::create(CCDelayTime::create(duration + 0.2f), call, nullptr));

    sprite->setOpacity(0);
    label->setOpacity(0);
    sprite->runAction(CCSequence::create(CCDelayTime::create(duration), CCFadeIn::create(0.3f), nullptr));
    label->runAction(CCSequence::create(CCDelayTime::create(duration), CCFadeIn::create(0.3f), nullptr));
}

void EndLevelLayer::diamondEnterFinished() {
    CCPoint pos = m_diamondsPosition + ccp(0.0f, 8.0f);

    CCParticleSystemQuad* particles = CCParticleSystemQuad::create("starEffect.plist");
    particles->onEnter();
    m_mainLayer->addChild(particles, 9);
    particles->setPosition(pos);

    CCCircleWave* wave = CCCircleWave::create(10.0f, 90.0f, 0.4f, false);
    wave->setColor(ccc3(255, 255, 255));
    wave->setPosition(pos);
    m_mainLayer->addChild(wave, 9);

    playEndEffect();
}

void EndLevelLayer::playCoinEffect(float duration) {
    if (!m_coinsToAnimate || m_coinsToAnimate->count() == 0) return;

    for (int i = 0; i < m_coinsToAnimate->count(); i++) {
        CCNode* coin = (CCNode*)m_coinsToAnimate->objectAtIndex(i);
        m_mainLayer->addChild(coin, 10);
        coin->setScale(3.0f);
        coin->setVisible(true);

        float delay = duration + i * 0.35f;

        CCCallFuncO* call = CCCallFuncO::create(this, callfuncO_selector(EndLevelLayer::coinEnterFinishedO), coin);

        CCScaleTo* scaleTo = CCScaleTo::create(0.4f, 1.0f);
        CCEaseBounceOut* ease = CCEaseBounceOut::create(scaleTo);
        CCFadeIn* fade = CCFadeIn::create(0.1f);

        coin->runAction(CCSequence::create(CCDelayTime::create(delay), CCSpawn::create(ease, fade, nullptr), nullptr));
        coin->runAction(CCSequence::create(CCDelayTime::create(delay + 0.1f), call, nullptr));
    }
}

void EndLevelLayer::coinEnterFinished(cocos2d::CCPoint position) {
    CCPoint pos = position;

    CCParticleSystemQuad* particles = CCParticleSystemQuad::create("starEffect.plist");
    particles->onEnter();
    m_mainLayer->addChild(particles, 9);
    particles->setPosition(pos);

    FMODAudioEngine::sharedEngine()->playEffect("highscoreGet02.ogg");

    bool isColored = m_notLocal && m_coinsVerified;
    CCCircleWave* wave = CCCircleWave::create(10.0f, 90.0f, 0.4f, false);
    wave->setColor(isColored ? ccc3(255, 255, 255) : ccc3(75, 175, 255));
    wave->setPosition(pos);
    m_mainLayer->addChild(wave, 9);
}

void EndLevelLayer::coinEnterFinishedO(cocos2d::CCObject* sender) {
    coinEnterFinished(sender->getPosition());
}

void EndLevelLayer::playEndEffect() {
    if (m_endEffectPlayed) return;
    m_endEffectPlayed = true;

    FMODAudioEngine::sharedEngine()->playEffect("gold02.ogg");

    if (m_secretKey) {
        FMODAudioEngine::sharedEngine()->playEffect("secretKey.ogg");
    } else if (m_orbs > 0) {
        FMODAudioEngine::sharedEngine()->playEffect("magicExplosion.ogg");
    }

    CCPoint pos = m_orbsPosition;
    if (m_orbs <= 0) {
        pos = m_starsPosition;
    }

    CurrencyRewardLayer* reward = CurrencyRewardLayer::create(
        m_orbs, m_stars, m_moons, m_diamonds,
        (CurrencySpriteType)(m_secretKey ? 9 : 0),
        m_secretKey, Icon, 0, ccp(0.0f, 1.0f),
        CurrencyRewardType::EndLevel, 1.0f, 1.0f);
    addChild(reward, 100);
}

void EndLevelLayer::onReplay(cocos2d::CCObject* sender) {
    if (m_exiting) return;
    m_exiting = true;

    if (!GameManager::sharedState()->getGameVariable("0024")) {
        PlatformToolbox::hideCursor();
    }

    FMODAudioEngine::sharedEngine()->clearAllAudio();
    FMODAudioEngine::sharedEngine()->playEffect("playSound_01.ogg", 1.0f, 0.0f, 0.3f);

    exitLayer(false);
    setTouchEnabled(false);
    setKeyboardEnabled(false);
    setKeypadEnabled(false);

    PlayLayer::delayedFullReset(m_playLayer);
}

void EndLevelLayer::onMenu(cocos2d::CCObject* sender) {
    if (m_exiting) return;
    m_exiting = true;

    if (m_playLayer->m_endLayerStars || m_animateCoins || m_playLayer->m_orbs > 0 || m_playLayer->m_diamonds > 0) {
        GameManager::sharedState()->doQuickSave();
    }

    PlayLayer::onQuit(m_playLayer);
    m_playLayer = nullptr;
    exitLayer(false);

    FMODAudioEngine::sharedEngine()->playEffect("quitSound_01.ogg", 1.0f, 0.0f, 0.7f);
}

void EndLevelLayer::onEdit(cocos2d::CCObject* sender) {
    if (m_exiting) return;
    m_exiting = true;

    m_playLayer->stopAllActions();
    m_playLayer->unscheduleAllSelectors();

    FMODAudioEngine::sharedEngine()->stopAllMusic(true);
    PlayLayer::removeAllObjects(m_playLayer);

    runAction(CCSequence::create(CCDelayTime::create(0.0f),
                                 CCCallFunc::create(this, callfunc_selector(EndLevelLayer::goEdit)),
                                 nullptr));
}

void EndLevelLayer::goEdit() {
    exitLayer(false);
    m_playLayer->stopAllActions();
    m_playLayer->unscheduleAllSelectors();
    GJBaseGameLayer::resetAudio(m_playLayer);

    CCDirector::sharedDirector()->getTouchDispatcher()->unregisterForcePrio(this);

    GJGameLoadingLayer* loadingLayer = GJGameLoadingLayer::transitionToLoadingLayer(m_playLayer->m_level, true);
    m_playLayer->m_loadingLayer = loadingLayer;
    m_playLayer = nullptr;
}

void EndLevelLayer::onRestartCheckpoint(cocos2d::CCObject* sender) {
    if (m_exiting) return;
    m_exiting = true;

    if (!GameManager::sharedState()->getGameVariable("0024")) {
        PlatformToolbox::hideCursor();
    }

    FMODAudioEngine::sharedEngine()->clearAllAudio();
    FMODAudioEngine::sharedEngine()->playEffect("playSound_01.ogg", 1.0f, 0.0f, 0.3f);

    exitLayer(false);
    setTouchEnabled(false);
    setKeyboardEnabled(false);
    setKeypadEnabled(false);

    CCSequence* seq = CCSequence::create(CCDelayTime::create(0.5f),
                                         CCCallFunc::create(m_playLayer, callfunc_selector(PlayLayer::onQuit)),
                                         nullptr);
    seq->setTag(16);
    m_playLayer->runAction(seq);
}

void EndLevelLayer::onHideLayer(cocos2d::CCObject* sender) {
    m_hidden = !m_hidden;

    m_mainLayer->stopActionByTag(12341);
    stopActionByTag(12341);

    CCPoint* target = m_hidden ? &m_startPosition : &m_endPosition;

    CCMoveTo* move = CCMoveTo::create(1.0f, *target);
    CCEaseInOut* ease = CCEaseInOut::create(move, 2.0f);
    ease->setTag(12341);
    m_mainLayer->runAction(ease);

    CCFadeTo* fade = CCFadeTo::create(1.0f, m_hidden ? 0 : 100);
    fade->setTag(12341);
    runAction(fade);
}

void EndLevelLayer::onLevelLeaderboard(cocos2d::CCObject* sender) {
    if (m_exiting || !m_playLayer) return;

    GJGameLevel* level = m_playLayer->m_level;
    LevelLeaderboardType type = (LevelLeaderboardType)GameManager::sharedState()->getIntGameVariable("0098");
    LevelLeaderboardMode mode = (LevelLeaderboardMode)GameManager::sharedState()->getIntGameVariable("0164");

    LevelLeaderboard* leaderboard = LevelLeaderboard::create(level, type, mode);
    leaderboard->show();
}

void EndLevelLayer::onEveryplay(cocos2d::CCObject* sender) {}

void EndLevelLayer::keyBackClicked() {
    onMenu(nullptr);
}

void EndLevelLayer::keyDown(cocos2d::enumKeyCodes key, double timestamp) {
    if (key == cocos2d::KEY_Space || key == cocos2d::CONTROLLER_A) {
        onReplay(nullptr);
        return;
    }
    if (key == cocos2d::KEY_Escape || key == cocos2d::CONTROLLER_B) {
        onMenu(nullptr);
        return;
    }
    cocos2d::CCLayer::keyDown(key, timestamp);
}

void EndLevelLayer::keyUp(cocos2d::enumKeyCodes key, double timestamp) {}

std::string EndLevelLayer::getCoinString() {
    if (!m_playLayer) return "";

    GJGameLevel* level = m_playLayer->m_level;
    std::string result = "<cy>all</c> coins";

    if (level->m_coins > 1) {
        bool has1 = (level->m_firstCoinVerified.random - level->m_firstCoinVerified.seed == 1);
        bool has2 = (level->m_secondCoinVerified.random - level->m_secondCoinVerified.seed == 1);
        bool has3 = (level->m_thirdCoinVerified.random - level->m_thirdCoinVerified.seed == 1);

        if (has1 || has2 || has3) {
            result = "coin ";
            int count = 0;

            if (has1) {
                result += "<cy>1</c>";
                count++;
            }
            if (has2) {
                if (count > 0) result += ", ";
                result += "<cy>2</c>";
                count++;
            }
            if (has3 && level->m_coins > 2) {
                if (count > 0) result += ", ";
                result += "<cy>3</c>";
            }
        }
    }

    return CCString::createWithFormat("You need to <cg>collect</c> %s to verify the level!", result.c_str())->getCString();
}

const char* EndLevelLayer::getEndText() {
    bool isUpsideDown = GameManager::sharedState()->getGameVariable("0095");

    if (isUpsideDown) {
        const char* messages[] = {
            "Took you long enough...", "Teach me oh great one", "Haxxor?",
            "Tripple spikes?", "RubRubRubRubRubRub", "SO FAST",
            "Hmmmmmmmmmmm", "Ship part was cool", "Timing could be better",
            "I cant hear the music.", "Pump. It. Up.", "I am Batman",
            "Take a break.", "AFK", "he protec", "Kappa", "lol is lol backwards",
            "Dr. Click", "Speedrun complete", "YEEEEEEEEEEEEEEEEEEES!",
            "Why are we here, just to suffer?", "HOW!", "Task failed successfully",
            "ok boomling", "I am not not impressed", "You are inevitable",
            "Sorry I fell asleep. Do it again?", "Did we win?", "Big brain moment",
            "E", "Nice song", "QUACK", "OMG Poggers", "pogchamp",
            "That's Sus", "BASED", "Fluked", "RubRub Approves", "NOICE",
            "You were not the Impostor", "I have the highground", "You got the W",
            "After a mental breakdown", "Not 1 attempt but ok",
            "Cool, now beat it with your eyes closed", "Some doubted... it happened",
            "Press alt + f4 for secret way", "Are ya winning son?", "el pepe",
            "LET'S ROCK IT!", "Well done... Now try to complete it without any checkpoints!",
            "You're cheating aren't you?", "YOU CANNOT VERIFY A LEVEL IF IT HAS A START POSITION!",
            "That doesnt count", "why am i argentina", "You are fuk map",
            "Robert Game", "You can now play as Luigi!", "these nuts",
            "What the BEEP was that?", "do it again I wasn't looking",
            "youre going to brazil", "Never Gonna Give You Up",
            "You cannot level a verify if the start pos has a checkpoint.",
            "You popped off fr.", "EMERGENCY MEETING!!!", "L", "huhu yo",
            "u have winned", "We're making balls ladies and gentleman",
            "I like ya cut G", "HOW", "Not impressive I beat that in like 2 seconds",
            "Does anyone read this?", "Uh oh, forgot to record", "Boy that's cap",
            "Time to go outside", "F in the chat", "Don't forget to thank the bus driver",
            "I can't believe you've done this", "Take a break!", "Update 2.3 coming soon",
            "She's in love with the concept...", "RobTop said funny messages, not cringe",
            "You're a pro? Name every Demon then", "poggie woggie",
            "Well yes, but actually no.", "Robala Topala approves",
            "Ropert Topert disapproves", "Never before have I seen such skill",
            "Listen here you little...", "kinda sloppy but ok u win",
            "This is why you have no friends", "BEHOLD, an endscreen comment!",
            "To be continued...", "Have you just... DASHED it???",
            "Why am I spending time on these messages", "Harder than FNF",
            "BUT BRO, DO YOU EVEN LIFT!?"
        };
        return messages[rand() % (sizeof(messages) / sizeof(messages[0]))];
    }

    const char* messages[] = {
        "Good Job!", "Well Done!", "Impressive!", "Amazing!",
        "Incredible!", "Skillful!", "Brilliant!",
        "You are... The One!", "How is this possible!?",
        "You beat me...", "Challenge Breaker!", "I am speechless...",
        "Reflex Master!", "Not bad!", "Warp Speed!", "Y u do dis?"
    };
    return messages[rand() % (sizeof(messages) / sizeof(messages[0]))];
}

void EndLevelLayer::tryShowBanner(float dt) {
    AdToolbox::enableBanner();
    unschedule(schedule_selector(EndLevelLayer::tryShowBanner));
}