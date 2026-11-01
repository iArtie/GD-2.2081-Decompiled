#pragma once

#include "Objects/EffectGameObject.h"
#include "Platform/cocos2d.h"
#include <string>

class GJBaseGameLayer;
class GameObject;

class ForceBlockGameObject : public EffectGameObject {
public:
    cocos2d::CCPoint calculateForceToTarget(GameObject* target);
    static ForceBlockGameObject* create(const char* frame);
    std::string getSaveString(GJBaseGameLayer* layer) override;
    
    virtual ~ForceBlockGameObject();

private:
    float m_force;
    float m_minForce;
    float m_maxForce;
    float m_objectRadius;
    int m_forceID;
    bool m_relativeForce;
    bool m_forceRange;
};