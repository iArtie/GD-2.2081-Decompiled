#pragma once

#include "cocos2d.h"
#include <string>

class GJBaseGameLayer;

class CameraTriggerGameObject : public EffectGameObject {
public:
    static CameraTriggerGameObject* create(const char* frame);
    std::string getSaveString(GJBaseGameLayer* layer);
    ~CameraTriggerGameObject();

private:
    int m_objectID;
    double m_duration;
    int m_easingType;
    double m_easingRate;
    double m_rotationDegrees;
    bool m_lockObjectRotation;
    bool m_isDirectionFollowSnap360;
    cocos2d::CCPoint m_moveOffset;
    double m_zoomValue;
    double m_previewOpacity;
    int m_targetGroupID;
    int m_edgeDirection;
    int m_centerGroupID;
    int m_moveTargetMode;
    bool m_exitStatic;
    bool m_followObject;
    double m_followEasing;
    bool m_smoothVelocity;
    double m_velocityModifier;
    bool m_exitInstant;
};