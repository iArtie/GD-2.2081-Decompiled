#include "ForceBlockGameObject.h"
#include "GJBaseGameLayer.h"
#include "GameObject.h"
#include <cmath>

cocos2d::CCPoint ForceBlockGameObject::calculateForceToTarget(GameObject* target) {
    cocos2d::CCRect rect = getObjectRect();
    float radius = m_objectRadius;
    
    if (radius <= 0.0f) {
        radius = (rect.size.width >= rect.size.height) ? rect.size.width * 0.5f : rect.size.height * 0.5f;
    }
    
    float angle = 0.0f;
    
    if (m_relativeForce) {
        cocos2d::CCPoint myPos = getRealPosition();
        cocos2d::CCPoint targetPos = target->getRealPosition();
        cocos2d::CCPoint diff = myPos - targetPos;
        angle = atan2(diff.y, diff.x);
        
        if (isFlipY()) {
            angle = angle + 0.0f;
        } else {
            angle = angle + 3.14159265f;
        }
    } else {
        float rotation = getRotationX();
        angle = (90.0f - rotation) * 0.017453f;
        if (isFlipY()) {
            angle += 3.14159265f;
        }
    }
    
    float forceScale = 1.0f;
    
    if (m_forceRange) {
        if (m_relativeForce) {
            cocos2d::CCPoint myPos = getRealPosition();
            cocos2d::CCPoint targetPos = target->getRealPosition();
            float distance = cocos2d::ccpDistance(myPos, targetPos);
            float normalized = (radius > distance) ? 1.0f - (distance / radius) : 0.0f;
            forceScale = cocos2d::clampf(normalized, 0.0f, 1.0f);
        } else {
            float cosA = cosf(angle);
            float sinA = sinf(angle);
            cocos2d::CCPoint dir = cocos2d::CCPoint(cosA, sinA);
            
            cocos2d::CCPoint myPos = getRealPosition();
            cocos2d::CCPoint targetPos = target->getRealPosition();
            cocos2d::CCPoint diff = myPos - targetPos;
            
            float dot = diff.x * dir.x + diff.y * dir.y;
            float normalized = (radius + dot) / radius;
            forceScale = cocos2d::clampf(normalized * 0.5f, 0.0f, 1.0f);
        }
        
        m_force = m_minForce + (m_maxForce - m_minForce) * forceScale;
    }
    
    cocos2d::CCPoint forceDir = cocos2d::ccpForAngle(angle);
    return forceDir * m_force;
}

ForceBlockGameObject* ForceBlockGameObject::create(const char* frame) {
    ForceBlockGameObject* ret = new ForceBlockGameObject();
    if (ret && ret->init(frame)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

std::string ForceBlockGameObject::getSaveString(GJBaseGameLayer* layer) {
    std::string saveString = EffectGameObject::getSaveString(layer);
    std::string result = saveString;
    
    if (m_force != 0.0f) {
        result += ",�," + std::to_string(m_force); // 0x95
    }
    
    if (m_minForce != 0.0f) {
        result += ",�," + std::to_string(m_minForce); // 0x20E
    }
    
    if (m_maxForce != 0.0f) {
        result += ",�," + std::to_string(m_maxForce); // 0x20F
    }
    
    if (m_relativeForce) {
        result += ",�," + std::to_string(m_relativeForce); // 0x210
    }
    
    if (m_forceRange) {
        result += ",�," + std::to_string(m_forceRange); // 0x211
    }
    
    if (m_forceID) {
        result += ",�," + std::to_string(m_forceID); // 0x212
    }
    
    return result;
}

ForceBlockGameObject::~ForceBlockGameObject() {
    // Base class destructor handles cleanup
}