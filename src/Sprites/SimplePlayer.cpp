#include "Sprites/SimplePlayer.h"
#include "Managers/GameManager.h"
#include "Utils/GameToolbox.h"
#include "Sprites/GJRobotSprite.h"
#include "Sprites/GJSpiderSprite.h"

void SimplePlayer::asyncLoadIcon(int id, IconType type) {
    if (GameManager::sharedState()->isIconLoaded(id, type)) {
        updatePlayerFrame(id, type);
    } else {
        m_iconLoaded = true;
        hideAll();
        GameManager::sharedState()->loadIconAsync(id, type, m_iconRequestID, this);
    }
}

SimplePlayer* SimplePlayer::create(int id) {
    SimplePlayer* returnVal = new SimplePlayer();
    if (returnVal->init(id)) {
        returnVal->autorelease();
        return returnVal;
    }
    delete returnVal;
    return nullptr;
}

void SimplePlayer::createRobotSprite(int frame) {
    if (!m_robotSprite) {
        m_robotSprite = GJRobotSprite::create(frame);
        addChild(m_robotSprite);
        m_robotSprite->setVisible(false);
    }
}

void SimplePlayer::createSpiderSprite(int frame) {
    if (!m_spiderSprite) {
        m_spiderSprite = GJSpiderSprite::create(frame);
        addChild(m_spiderSprite);
        m_spiderSprite->setVisible(false);
        m_spiderSprite->setPosition(cocos2d::CCPoint(0.0f, -1.5f));
    }
}

void SimplePlayer::disableCustomGlowColor() {
    m_hasCustomGlowColor = false;
}

void SimplePlayer::enableCustomGlowColor(const cocos2d::ccColor3B* color) {
    m_hasCustomGlowColor = true;
    m_glowColor = *color;
}

void SimplePlayer::hideAll() {
    m_firstLayer->setVisible(false);
    m_secondLayer->setVisible(false);
    if (m_birdDome)
        m_birdDome->setVisible(false);
    if (m_outlineSprite)
        m_outlineSprite->setVisible(false);
    if (m_detailSprite)
        m_detailSprite->setVisible(false);
    if (m_robotSprite)
        m_robotSprite->setVisible(false);
    if (m_spiderSprite)
        m_spiderSprite->setVisible(false);
}

void SimplePlayer::hideSecondary() {
    m_secondLayer->setVisible(false);
    m_birdDome->setVisible(false);
    m_detailSprite->setVisible(false);
    m_outlineSprite->setVisible(false);
    if (m_robotSprite) {
        m_robotSprite->hideSecondary();
        m_robotSprite->hideGlow();
    }
    if (m_spiderSprite) {
        m_spiderSprite->hideSecondary();
        m_spiderSprite->hideGlow();
    }
}

void SimplePlayer::iconFinishedLoading(int id, IconType type) {
    m_iconLoaded = false;
    updatePlayerFrame(id, type);
    setOpacity(0);
    runAction(cocos2d::CCFadeIn::create(0.1f));
}

bool SimplePlayer::init(int id) {
    m_iconRequestID = GameManager::sharedState()->getIconRequestID();

    if (!cocos2d::CCSprite::init())
        return false;

    setTextureRect(cocos2d::CCRect(0, 0, 0, 0));

    int frameID = (id > 0) ? ((id > 485) ? 485 : id) : 1;
    std::string baseName = "player_" + std::to_string(frameID);
    std::string frame1 = baseName + "_001.png";
    std::string frame2 = baseName + "_2_001.png";
    std::string frame3 = baseName + "_glow_001.png";

    GameManager::sharedState()->loadIcon(frameID, IconType::Cube, m_iconRequestID);

    m_firstLayer = cocos2d::CCSprite::createWithSpriteFrameName(frame1.c_str());
    addChild(m_firstLayer, 1);

    m_secondLayer = cocos2d::CCSprite::createWithSpriteFrameName(frame2.c_str());
    m_firstLayer->addChild(m_secondLayer, -1);
    m_secondLayer->setPosition(cocos2d::CCNode::convertToNodeSpace(m_firstLayer, &cocos2d::CCPoint(0, 0)));

    m_birdDome = cocos2d::CCSprite::createWithSpriteFrameName(frame2.c_str());
    m_firstLayer->addChild(m_birdDome, -2);
    m_birdDome->setPosition(cocos2d::CCNode::convertToNodeSpace(m_firstLayer, &cocos2d::CCPoint(0, 0)));

    m_outlineSprite = cocos2d::CCSprite::createWithSpriteFrameName(frame3.c_str());
    m_firstLayer->addChild(m_outlineSprite, -3);
    m_outlineSprite->setPosition(cocos2d::CCNode::convertToNodeSpace(m_firstLayer, &cocos2d::CCPoint(0, 0)));
    m_outlineSprite->setVisible(false);

    m_detailSprite = cocos2d::CCSprite::createWithSpriteFrameName(frame2.c_str());
    m_firstLayer->addChild(m_detailSprite, 1);
    m_detailSprite->setPosition(cocos2d::CCNode::convertToNodeSpace(m_firstLayer, &cocos2d::CCPoint(0, 0)));

    updatePlayerFrame(frameID, IconType::Cube);
    return true;
}

void SimplePlayer::setColor(const cocos2d::ccColor3B* color) {
    m_firstLayer->setColor(*color);
    updateColors();
}

void SimplePlayer::setColors(const cocos2d::ccColor3B* color1, const cocos2d::ccColor3B* color2) {
    m_firstLayer->setColor(*color1);
    m_secondLayer->setColor(*color2);
    updateColors();
}

void SimplePlayer::setOpacity(unsigned char opacity) {
    m_firstLayer->setOpacity(opacity);
    m_secondLayer->setOpacity(opacity);
    m_birdDome->setOpacity(opacity);
    m_outlineSprite->setOpacity(opacity);
    m_detailSprite->setOpacity(opacity);
    if (m_robotSprite)
        m_robotSprite->setOpacity(opacity);
    if (m_spiderSprite)
        m_spiderSprite->setOpacity(opacity);
}

void SimplePlayer::setSecondColor(const cocos2d::ccColor3B* color) {
    m_secondLayer->setColor(*color);
    updateColors();
}

void SimplePlayer::tryEnableCustomGlowColor(int id) {
    if (id < 0) {
        disableCustomGlowColor();
    } else {
        cocos2d::ccColor3B color = GameManager::sharedState()->colorForIdx(id);
        enableCustomGlowColor(&color);
    }
}

void SimplePlayer::updateColors() {
    cocos2d::ccColor3B color1 = m_firstLayer->getColor();
    cocos2d::ccColor3B color2 = m_secondLayer->getColor();
    cocos2d::ccColor3B glowColor;

    if (color2.r == 0 && color2.g == 0 && color2.b == 0) {
        if (color1.r == 0 && color1.g == 0 && color1.b == 0)
            glowColor = {255, 255, 255};
        else
            glowColor = color1;
    } else {
        if (color1.r == 0 && color1.g == 0 && color1.b == 0)
            glowColor = color2;
        else
            glowColor = color2;
    }
    glowColor = GameToolbox::strongColor(glowColor);
    if (m_hasCustomGlowColor)
        glowColor = m_glowColor;

    m_outlineSprite->setColor(glowColor);
    m_outlineSprite->setVisible(true);

    if (m_robotSprite) {
        m_robotSprite->updateColor01(color1);
        m_robotSprite->updateColor02(color2);
        m_robotSprite->updateGlowColor(glowColor, true);
    }
    if (m_spiderSprite) {
        m_spiderSprite->updateColor01(color1);
        m_spiderSprite->updateColor02(color2);
        m_spiderSprite->updateGlowColor(glowColor, true);
    }

    if ((color1.r == 0 && color1.g == 0 && color1.b == 0) || m_hasGlowOutline) {
        if (m_robotSprite)
            m_robotSprite->showGlow();
        if (m_spiderSprite)
            m_spiderSprite->showGlow();
    } else {
        if (m_robotSprite)
            m_robotSprite->hideGlow();
        if (m_spiderSprite)
            m_spiderSprite->hideGlow();
    }
}

void SimplePlayer::updatePlayerFrame(int id, IconType type) {
    std::string prefix;
    int maxFrame;
    switch (type) {
        case IconType::Ship:    prefix = "ship";        maxFrame = 169; break;
        case IconType::Ball:    prefix = "player_ball"; maxFrame = 118; break;
        case IconType::Ufo:     prefix = "bird";        maxFrame = 149; break;
        case IconType::Wave:    prefix = "dart";        maxFrame = 96;  break;
        case IconType::Robot:   prefix = "robot";       maxFrame = 68;  break;
        case IconType::Spider:  prefix = "spider";      maxFrame = 69;  break;
        case IconType::Swing:   prefix = "swing";       maxFrame = 43;  break;
        case IconType::Jetpack: prefix = "jetpack";     maxFrame = 8;   break;
        default:                prefix = "player";      maxFrame = 485; break;
    }

    if (m_robotSprite)
        m_robotSprite->setVisible(type == IconType::Robot);
    if (m_spiderSprite)
        m_spiderSprite->setVisible(type == IconType::Spider);
    m_firstLayer->setVisible(type != IconType::Robot && type != IconType::Spider);
    m_secondLayer->setVisible(type != IconType::Robot && type != IconType::Spider);
    m_birdDome->setVisible(type != IconType::Robot && type != IconType::Spider);

    int frameID = (id > 0) ? ((id > maxFrame) ? maxFrame : id) : 1;

    GameManager::sharedState()->loadIcon(frameID, type, m_iconRequestID);

    if (type == IconType::Robot) {
        if (!m_robotSprite)
            createRobotSprite(frameID);
        else
            m_robotSprite->updateFrame(frameID);
        m_robotSprite->setVisible(true);
        return;
    }
    if (type == IconType::Spider) {
        if (!m_spiderSprite)
            createSpiderSprite(frameID);
        else
            m_spiderSprite->updateFrame(frameID);
        m_spiderSprite->setVisible(true);
        return;
    }

    std::string frame1 = cocos2d::CCString::createWithFormat("%s_%02d_001.png", prefix.c_str(), frameID)->getCString();
    std::string frame2 = cocos2d::CCString::createWithFormat("%s_%02d_2_001.png", prefix.c_str(), frameID)->getCString();
    std::string frame3;
    if (type == IconType::Ufo)
        frame3 = cocos2d::CCString::createWithFormat("%s_%02d_3_001.png", prefix.c_str(), frameID)->getCString();
    std::string frameGlow = cocos2d::CCString::createWithFormat("%s_%02d_glow_001.png", prefix.c_str(), frameID)->getCString();
    std::string frameExtra = cocos2d::CCString::createWithFormat("%s_%02d_extra_001.png", prefix.c_str(), frameID)->getCString();

    setFrames(frame1.c_str(), frame2.c_str(), frame3.empty() ? nullptr : frame3.c_str(),
              frameGlow.c_str(), frameExtra.c_str());

    m_firstLayer->setScale(1.0f);
    if (type == IconType::Ball) {
        m_firstLayer->setScale(0.9f);
    } else if (type == IconType::Ufo) {
        m_firstLayer->setPosition(cocos2d::CCPoint(0.0f, -7.0f));
    } else {
        m_firstLayer->setPosition(cocos2d::CCPoint(0.0f, 0.0f));
    }
}

SimplePlayer::~SimplePlayer() {
}