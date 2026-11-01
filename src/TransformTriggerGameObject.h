#pragma once

#include "cocos2d.h"
#include "EffectGameObject.h"
#include <string>

class GJBaseGameLayer;

class TransformTriggerGameObject : public EffectGameObject {
public:
    static TransformTriggerGameObject* create(const char* frame);
    bool init(const char* frame) override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    virtual ~TransformTriggerGameObject();

protected:
    int m_targetGroupID = 0;
    int m_centerGroupID = 0;
    double m_duration = 0.0;
    int m_easingType = 0;
    double m_easingRate = 0.0;
    double m_objectScaleX = 0.0;
    double m_objectScaleY = 0.0;
    double m_property450 = 0.0;
    double m_property451 = 0.0;
    bool m_divideX = false;
    bool m_divideY = false;
    bool m_onlyMove = false;
    bool m_relativeRotation = false;
    bool m_relativeScale = false;
};