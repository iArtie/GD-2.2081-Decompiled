#include "SmartGameObject.h"
#include "GJBaseGameLayer.h"

SmartGameObject* SmartGameObject::create(const char* frame) {
    SmartGameObject* returnVal = new SmartGameObject();
    if (returnVal->init(frame)) {
        returnVal->autorelease();
    } else {
        delete returnVal;
        returnVal = nullptr;
    }
    return returnVal;
}

std::string SmartGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = GameObject::getSaveString(layer);
    if (m_referenceOnly) {
        saveString += ",";
        saveString += std::to_string(157);
        saveString += ",";
        saveString += std::to_string(1);
    }
    return saveString;
}

bool SmartGameObject::init(const char* frame) {
    if (!GameObject::init(frame))
        return false;
    m_smartFrame = frame;
    m_bUnkBool2 = false;
    m_classType = Smart;
    return true;
}

void SmartGameObject::updateSmartFrame() {
    const char* newFrame;
    switch (m_objectID) {
        case 2896:
            newFrame = m_referenceOnly ? "smartBlock02b_001.png" : "smartBlock02_001.png";
            break;
        case 2897:
            newFrame = m_referenceOnly ? "smartBlock03b_001.png" : "smartBlock03_001.png";
            break;
        case 2895:
            newFrame = m_referenceOnly ? "smartBlock01b_001.png" : "smartBlock01_001.png";
            break;
        default:
            return;
    }
    if (m_smartFrame != newFrame) {
        m_smartFrame = newFrame;
        setDisplayFrame(cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(newFrame));
    }
}

SmartGameObject::~SmartGameObject() {
}