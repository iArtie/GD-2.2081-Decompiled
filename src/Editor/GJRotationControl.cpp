#include "Editor/GJRotationControl.h"
#include "Editor/GJRotationControlDelegate.h"

GJRotationControl* GJRotationControl::create() {
    auto control = new GJRotationControl();
    if (control && control->init()) {
        control->autorelease();
        return control;
    }
    delete control;
    return nullptr;
}

bool GJRotationControl::init() {
    if (!CCLayer::init()) {
        return false;
    }
    
    setTouchEnabled(true);
    
    m_controlPosition = cocos2d::CCPoint(60.0f, 0.0f);
    setAnchorPoint(cocos2d::CCPoint(0.5f, 0.5f));
    m_touchID = -1;
    m_currentRotation = 0.0f;
    m_startingRotation = 0.0f;
    m_delegate = nullptr;
    
    m_controlSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_rotationControlBtn01_001.png");
    addChild(m_controlSprite);
    setAngle(0.0f);
    
    return true;
}

bool GJRotationControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touchID != -1) {
        return false;
    }
    
    cocos2d::CCPoint location = touch->getLocation();
    cocos2d::CCPoint local = convertToNodeSpace(location);
    
    if (local.getDistance(m_controlPosition) < 24.0f) {
        m_startingRotation = m_currentRotation;
        m_cursorDifference = local - m_controlPosition;
        m_touchID = touch->getID();
        
        if (m_delegate) {
            m_delegate->angleChangeBegin();
        }
        
        m_controlSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_rotationControlBtn02_001.png")
        );
        
        return true;
    }
    
    return false;
}

void GJRotationControl::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touchID != touch->getID()) {
        return;
    }
    
    cocos2d::CCPoint location = touch->getLocation();
    cocos2d::CCPoint local = convertToNodeSpace(location);
    updateSliderPosition(local);
}

void GJRotationControl::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (m_touchID == touch->getID()) {
        finishTouch();
    }
}

void GJRotationControl::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    ccTouchEnded(touch, event);
}

void GJRotationControl::finishTouch() {
    if (m_touchID != -1) {
        m_touchID = -1;
        
        m_controlSprite->setDisplayFrame(
            cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("GJ_rotationControlBtn01_001.png")
        );
        
        if (m_delegate) {
            m_delegate->angleChangeEnded();
        }
    }
}

void GJRotationControl::setAngle(float angle) {
    float rad = -angle * 0.017453f;
    m_startingRotation = angle;
    m_currentRotation = angle;
    
    m_controlPosition = cocos2d::CCPoint(cosf(rad) * 60.0f, sinf(rad) * 60.0f);
    m_controlSprite->setPosition(m_controlPosition);
}

void GJRotationControl::updateSliderPosition(cocos2d::CCPoint position) {
    cocos2d::CCPoint delta = position - m_cursorDifference;
    float angle = atan2f(delta.y, delta.x);
    
    m_controlPosition = cocos2d::CCPoint(cosf(angle) * 60.0f, sinf(angle) * 60.0f);
    m_controlSprite->setPosition(m_controlPosition);
    
    float rotation = roundf(-angle * 57.296f * 1000.0f) / 1000.0f;
    
    if (m_currentRotation != rotation) {
        float deltaAngle = rotation - m_currentRotation;
        m_currentRotation = rotation;
        
        if (m_delegate) {
            m_delegate->angleChanged(deltaAngle);
        }
    }
}

void GJRotationControl::draw() {
    cocos2d::ccDrawColor4B(255, 255, 255, 50);
    glLineWidth(1.0f);
    cocos2d::ccDrawCircle(cocos2d::CCPoint(0.0f, 0.0f), 60.0f, 0.0f, 0, false, 1.0f, 1.0f);
}

GJRotationControl::~GJRotationControl() {
}