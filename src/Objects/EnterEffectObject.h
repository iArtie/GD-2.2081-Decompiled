#pragma once

#include "Objects/EffectGameObject.h"
#include <string>

class GJBaseGameLayer;

struct HSVValue {
    float h;
    float s;
    float v;
    bool absoluteSaturation;
    bool absoluteBrightness;
};

class EnterEffectObject : public EffectGameObject {
public:
    static EnterEffectObject* create(const char* frame);
    
    void customSetup() override;
    std::string getSaveString(GJBaseGameLayer* layer) override;
    bool init(const char* frame) override;
    void resetEnterAnimValues();
    
    virtual ~EnterEffectObject();

private:
    int m_length;
    int m_lengthVariance;
    int m_offset;
    int m_offsetVariance;
    int m_offsetY;
    int m_offsetYVariance;
    int m_moveDistance;
    int m_moveDistanceVariance;
    int m_moveAngle;
    int m_moveAngleVariance;
    int m_moveX;
    int m_moveXVariance;
    int m_moveY;
    int m_moveYVariance;
    
    float m_modFront;
    float m_modBack;
    float m_deadzone;
    float m_areaScaleX;
    float m_areaScaleXVariance;
    float m_areaScaleY;
    float m_areaScaleYVariance;
    float m_areaRotation;
    float m_areaRotationVariance;
    float m_areaTint;
    float m_property285;
    float m_toOpacity;
    float m_fromOpacity;
    float m_relativeFade;
    float m_duration;
    float m_easingInRate;
    float m_easingOutRate;
    
    int m_objectType;
    int m_unk390;
    int m_objectID;
    int m_enterType;
    int m_enterChannel;
    int m_easingInType;
    int m_easingOutType;
    int m_relative;
    int m_tintChannelID;
    int m_property224;
    int m_directionType;
    int m_targetGroupID;
    int m_centerGroupID;
    int m_specialTarget;
    int m_xyMode;
    int m_easeOutEnabled;
    int m_effectID;
    int m_inbound;
    int m_targetPlayer1;
    int m_targetPlayer2;
    int m_followCPP;
    int m_pulseMainOnly;
    int m_pulseDetailOnly;
    int m_twoDirections;
    int m_priority;
    int m_hsvEnabled;
    int m_useEffectID;
    int m_dontEditAreaParent;
    
    HSVValue m_hsvValue;
    
    cocos2d::CCPoint m_unkPoint1;
    cocos2d::CCPoint m_unkPoint2;
    bool m_unkBool1;
    int m_unkInt1;
};