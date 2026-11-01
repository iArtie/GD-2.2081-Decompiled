#include "Models/GJMapObject.h"
#include "Objects/GameObject.h"
#include "Managers/SpriteAnimationManager.h"
#include "Cocos/CCAnimatedSprite.h"

GJMapObject* GJMapObject::create(cocos2d::CCNode* parent) {
    auto object = new GJMapObject();
    if (object && object->init(parent)) {
        object->autorelease();
        return object;
    }
    delete object;
    return nullptr;
}

GJMapObject* GJMapObject::createMonster(int objectID) {
    float scale = (objectID == 918) ? (rand() * 4.6566e-10f * 0.4f + 0.7f) : 1.0f;
    
    auto gameObject = GameObject::createWithKey(objectID);
    gameObject->updateMainColor(cocos2d::ccc3(0, 0, 0));
    gameObject->updateSecondaryColor(cocos2d::ccc3(255, 255, 255));
    
    auto animManager = gameObject->getSpriteAnimationManager();
    animManager->runAnimation(gameObject->getAnimationString());
    
    if (objectID == 918) {
        animManager->updateAnimationSpeed(0.9f);
        gameObject->setRotation(rand() * 4.6566e-10f * 10.0f + 10.0f);
        gameObject->setScale(scale * 0.35f);
        
        auto mapObject = GJMapObject::create(gameObject);
        mapObject->updateShadow(scale * 0.18f, scale * 0.5f, cocos2d::CCPoint(0.0f, -11.0f * scale));
        
        auto delay = cocos2d::CCDelayTime::create(rand() * 4.6566e-10f * 0.4f);
        auto call = cocos2d::CCCallFunc::create(mapObject, callfunc_selector(GJMapObject::startMonsterJumpLoop));
        auto sequence = cocos2d::CCSequence::create(delay, call, nullptr);
        mapObject->runAction(sequence);
        
        return mapObject;
    }
    
    if (objectID == 1327) {
        gameObject->setScale(scale * 0.6f);
    }
    
    return GJMapObject::create(gameObject);
}

bool GJMapObject::init(cocos2d::CCNode* parent) {
    if (!parent || !CCNode::init()) {
        return false;
    }
    
    m_mainNode = parent;
    m_mainNode->retain();
    addChild(m_mainNode, 2);
    
    m_shadowSprite = cocos2d::CCSprite::createWithSpriteFrameName("chest_shadow_001.png");
    addChild(m_shadowSprite, 1);
    m_shadowSprite->setScaleX(0.16f);
    m_shadowSprite->setScaleY(0.45f);
    m_shadowSprite->setOpacity(50);
    m_shadowSprite->setPosition(cocos2d::CCPoint(0.0f, -9.0f));
    
    return true;
}

void GJMapObject::monsterIdle() {
    stopActionByTag(1);
    m_monsterMoving = false;
    m_monsterIdle = true;
    
    auto animatedSprite = static_cast<CCAnimatedSprite*>(m_mainNode);
    animatedSprite->tweenToAnimation("idle01", 0.3f);
    
    auto moveUp = cocos2d::CCMoveBy::create(0.2f, cocos2d::CCPoint(0.0f, -4.0f));
    auto easeOut = cocos2d::CCEaseOut::create(moveUp, 1.6f);
    auto moveDown = cocos2d::CCMoveBy::create(0.2f, cocos2d::CCPoint(0.0f, 4.0f));
    auto easeIn = cocos2d::CCEaseIn::create(moveDown, 1.6f);
    auto moveUp2 = cocos2d::CCMoveBy::create(0.2f, cocos2d::CCPoint(0.0f, 2.0f));
    auto easeOut2 = cocos2d::CCEaseOut::create(moveUp2, 1.8f);
    auto moveDown2 = cocos2d::CCMoveBy::create(0.2f, cocos2d::CCPoint(0.0f, -2.0f));
    auto easeInOut = cocos2d::CCEaseInOut::create(moveDown2, 2.0f);
    
    auto moveSequence = cocos2d::CCSequence::create(easeOut, easeIn, easeOut2, easeInOut, nullptr);
    m_mainNode->runAction(moveSequence);
    
    float currentScaleX = m_shadowSprite->getScaleX();
    auto scaleDown = cocos2d::CCScaleTo::create(0.4f, currentScaleX * 0.85f);
    auto easeScaleDown = cocos2d::CCEaseInOut::create(scaleDown, 1.8f);
    auto scaleUp = cocos2d::CCScaleTo::create(0.4f, currentScaleX);
    auto easeScaleUp = cocos2d::CCEaseInOut::create(scaleUp, 1.8f);
    auto scaleSequence = cocos2d::CCSequence::create(easeScaleDown, easeScaleUp, nullptr);
    m_shadowSprite->runAction(scaleSequence);
    
    float delayTime = rand() * 4.6566e-10f * 5.0f + 1.0f;
    auto delay = cocos2d::CCDelayTime::create(delayTime);
    auto call = cocos2d::CCCallFunc::create(this, callfunc_selector(GJMapObject::startMonsterJumpLoop));
    auto sequence = cocos2d::CCSequence::create(delay, call, nullptr);
    runAction(sequence);
}

void GJMapObject::monsterJump() {
    auto jumpUp = cocos2d::CCMoveBy::create(0.2025f, cocos2d::CCPoint(0.0f, 16.0f));
    auto easeOut = cocos2d::CCEaseOut::create(jumpUp, 1.6f);
    auto jumpDown = cocos2d::CCMoveBy::create(0.2025f, cocos2d::CCPoint(0.0f, -16.0f));
    auto easeIn = cocos2d::CCEaseIn::create(jumpDown, 1.6f);
    
    float currentTime = getCurrentTime();
    cocos2d::CCCallFunc* callFunc;
    if (currentTime <= m_jumpStopTime) {
        callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(GJMapObject::monsterJump));
    } else {
        callFunc = cocos2d::CCCallFunc::create(this, callfunc_selector(GJMapObject::monsterIdle));
    }
    
    auto delay = cocos2d::CCDelayTime::create(0.01f);
    auto sequence = cocos2d::CCSequence::create(easeOut, easeIn, delay, callFunc, nullptr);
    m_mainNode->runAction(sequence);
    
    float currentScaleX = m_shadowSprite->getScaleX();
    auto scaleUp = cocos2d::CCScaleTo::create(0.2025f, currentScaleX * 1.3f);
    auto easeScaleUp = cocos2d::CCEaseInOut::create(scaleUp, 1.8f);
    auto scaleDown = cocos2d::CCScaleTo::create(0.2025f, currentScaleX);
    auto easeScaleDown = cocos2d::CCEaseInOut::create(scaleDown, 1.8f);
    auto scaleSequence = cocos2d::CCSequence::create(easeScaleUp, easeScaleDown, nullptr);
    m_shadowSprite->runAction(scaleSequence);
    
    if (!m_monsterMoving && currentTime > m_moveStopTime) {
        moveMonster();
    }
}

void GJMapObject::moveFinished() {
    m_monsterMoving = false;
    m_moveStopTime = getCurrentTime() + rand() * 4.6566e-10f * 5.0f;
}

void GJMapObject::moveMonster() {
    cocos2d::CCPoint target = m_objectOrigin;
    target.x += m_objectOffset;
    // move monster to target position
}

void GJMapObject::playerJump() {
    float currentScale = m_mainNode->getScale();
    
    auto scaleDown = cocos2d::CCScaleTo::create(0.125f, currentScale * 0.8f);
    auto easeScaleDown = cocos2d::CCEaseInOut::create(scaleDown, 2.0f);
    auto scaleUp = cocos2d::CCScaleTo::create(0.3125f, currentScale * 1.1f);
    auto easeScaleUp = cocos2d::CCEaseInOut::create(scaleUp, 2.0f);
    auto scaleNormal = cocos2d::CCScaleTo::create(0.1875f, currentScale);
    auto easeScaleNormal = cocos2d::CCEaseInOut::create(scaleNormal, 2.0f);
    auto scaleSequence = cocos2d::CCSequence::create(easeScaleDown, easeScaleUp, easeScaleNormal, nullptr);
    m_mainNode->runAction(scaleSequence);
    
    auto jumpUp = cocos2d::CCMoveBy::create(0.225f, cocos2d::CCPoint(0.0f, 20.0f));
    auto easeOut = cocos2d::CCEaseOut::create(jumpUp, 1.8f);
    auto jumpDown = cocos2d::CCMoveBy::create(0.225f, cocos2d::CCPoint(0.0f, -20.0f));
    auto easeIn = cocos2d::CCEaseIn::create(jumpDown, 1.8f);
    auto moveSequence = cocos2d::CCSequence::create(easeOut, easeIn, nullptr);
    m_mainNode->runAction(moveSequence);
    
    float shadowScale = m_shadowSprite->getScaleX();
    auto shadowUp = cocos2d::CCScaleTo::create(0.2475f, shadowScale * 1.3f);
    auto easeShadowUp = cocos2d::CCEaseInOut::create(shadowUp, 2.0f);
    auto shadowDown = cocos2d::CCScaleTo::create(0.2475f, shadowScale);
    auto easeShadowDown = cocos2d::CCEaseInOut::create(shadowDown, 2.0f);
    auto shadowSequence = cocos2d::CCSequence::create(easeShadowUp, easeShadowDown, nullptr);
    m_shadowSprite->runAction(shadowSequence);
    
    auto delay = cocos2d::CCDelayTime::create(1.0f);
    auto call = cocos2d::CCCallFunc::create(this, callfunc_selector(GJMapObject::playerJump));
    auto sequence = cocos2d::CCSequence::create(delay, call, nullptr);
    runAction(sequence);
}

void GJMapObject::setObjectOrigin(cocos2d::CCPoint position, float offset) {
    m_objectOffset = offset;
    m_objectOrigin = position;
    setPosition(position);
}

void GJMapObject::startMonsterJumpLoop() {
    moveFinished();
    m_monsterIdle = false;
    
    float currentTime = getCurrentTime();
    int randVal = rand();
    m_jumpStopTime = currentTime + 5.0f + randVal * 4.6566e-10f * 10.0f;
    
    auto animManager = static_cast<CCAnimatedSprite*>(m_mainNode)->getSpriteAnimationManager();
    animManager->runAnimation(static_cast<CCAnimatedSprite*>(m_mainNode)->getAnimationString());
    animManager->updateAnimationSpeed(0.9f);
    
    monsterJump();
}

void GJMapObject::startPlayerJumpLoop() {
    auto delay = cocos2d::CCDelayTime::create(1.0f);
    auto call = cocos2d::CCCallFunc::create(this, callfunc_selector(GJMapObject::playerJump));
    auto sequence = cocos2d::CCSequence::create(delay, call, nullptr);
    runAction(sequence);
}

void GJMapObject::touchMonster() {
    ++m_monstersTouched;
    
    if (m_monstersTouched <= 9) {
        auto animatedSprite = static_cast<CCAnimatedSprite*>(m_mainNode);
        animatedSprite->stopActionByTag(2);
        
        auto tintRed = cocos2d::CCTintTo::create(0.05f, 255, 0, 0);
        auto tintNormal = cocos2d::CCTintTo::create(0.5f, 0, 0, 0);
        auto sequence = cocos2d::CCSequence::create(tintRed, tintNormal, nullptr);
        sequence->setTag(2);
        animatedSprite->runAction(sequence);
    }
}

GJMapObject::~GJMapObject() {
    if (m_mainNode) {
        m_mainNode->release();
    }
}